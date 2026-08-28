#!/usr/bin/env python3
"""
PalmAI OpenAPI 接口调用工具

用法:
    python3 call_api.py --secret-id <SECRET_ID> --private-key-file <PEM_FILE> \
                        --host <HOST> --uri <URI> --payload <JSON_BODY> \
                        [--encrypt-key-file <PEM_FILE>] [--encrypt-fields <FIELDS>]

示例:
    # 查询录掌状态
    python3 call_api.py --secret-id "Aeon-SecretId" \
                        --private-key-file private-key.pem \
                        --host "open-hk.palm.tencent.com" \
                        --uri "/cgi-bin/get-user-palm" \
                        --payload '{"user_id":"test-user"}'

    # 带加密字段的请求
    python3 call_api.py --secret-id "Aeon-SecretId" \
                        --private-key-file private-key.pem \
                        --host "open-hk.palm.tencent.com" \
                        --uri "/cgi-bin/user-palm-register" \
                        --payload '{"user_id":"test","user_name":"张三","phone_no":"13800138000"}' \
                        --encrypt-key-file encrypt-public-key.pem \
                        --encrypt-fields "user_name,phone_no"
"""

import argparse
import hashlib
import time
import base64
import json
import sys
import urllib.request
import urllib.error
import ssl

try:
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import padding as asym_padding
except ImportError:
    print("错误: 需要安装 cryptography 库")
    print("运行: pip3 install cryptography")
    sys.exit(1)

ALGORITHM = "SHA256withRSA"


def sha256_hex(s: str) -> str:
    return hashlib.sha256(s.encode("utf-8")).hexdigest()


def rsa_sign(message: str, private_key_pem: str) -> str:
    private_key = serialization.load_pem_private_key(
        private_key_pem.encode(), password=None
    )
    signature = private_key.sign(
        message.encode("utf-8"),
        asym_padding.PKCS1v15(),
        hashes.SHA256()
    )
    return base64.b64encode(signature).decode()


def rsa_oaep_encrypt(plaintext: str, public_key_pem: str) -> str:
    """使用RSA-OAEP-SHA256加密"""
    public_key = serialization.load_pem_public_key(public_key_pem.encode())
    ciphertext = public_key.encrypt(
        plaintext.encode("utf-8"),
        asym_padding.OAEP(
            mgf=asym_padding.MGF1(algorithm=hashes.SHA256()),
            algorithm=hashes.SHA256(),
            label=None
        )
    )
    return base64.b64encode(ciphertext).decode()


def gen_authorization(secret_id: str, private_key_pem: str,
                      host: str, uri: str, payload: str,
                      timestamp: str) -> str:
    http_request_method = "POST"
    canonical_query_string = ""
    canonical_headers = f"content-type:application/json\nhost:{host}\n"
    signed_headers = "content-type;host"
    hashed_request_payload = sha256_hex(payload)

    canonical_request = (
        f"{http_request_method}\n{uri}\n{canonical_query_string}\n"
        f"{canonical_headers}\n{signed_headers}\n{hashed_request_payload}"
    )

    hashed_canonical_request = sha256_hex(canonical_request)
    string_to_sign = f"{ALGORITHM}\n{timestamp}\n{hashed_canonical_request}"
    signature = rsa_sign(string_to_sign, private_key_pem)

    return (
        f"{ALGORITHM} Credential={secret_id}, "
        f"SignedHeaders={signed_headers}, "
        f"Signature={signature}"
    )


def encrypt_payload_fields(payload: str, encrypt_key_pem: str, encrypt_fields: list) -> str:
    """对payload中指定的敏感字段进行RSA-OAEP加密"""
    payload_dict = json.loads(payload)
    for field in encrypt_fields:
        if field in payload_dict and payload_dict[field]:
            payload_dict[field] = rsa_oaep_encrypt(str(payload_dict[field]), encrypt_key_pem)
    return json.dumps(payload_dict, ensure_ascii=False)


def call_api(secret_id: str, private_key_pem: str, host: str, uri: str,
             payload: str, encrypt_key_pem: str = None,
             encrypt_fields: list = None) -> dict:
    """调用API并返回结果"""

    # 加密指定字段
    if encrypt_key_pem and encrypt_fields:
        payload = encrypt_payload_fields(payload, encrypt_key_pem, encrypt_fields)

    timestamp = str(int(time.time()))
    authorization = gen_authorization(secret_id, private_key_pem, host, uri, payload, timestamp)

    # 构建请求
    url = f"https://{host}{uri}"
    headers = {
        "Authorization": authorization,
        "Content-Type": "application/json",
        "Host": host,
        "X-TC-Timestamp": timestamp,
    }

    req = urllib.request.Request(url, data=payload.encode("utf-8"), headers=headers, method="POST")

    # 发送请求
    ctx = ssl.create_default_context()
    try:
        with urllib.request.urlopen(req, context=ctx, timeout=30) as response:
            response_body = response.read().decode("utf-8")
            try:
                body = json.loads(response_body) if response_body else {}
            except (json.JSONDecodeError, ValueError):
                body = {"raw_body": response_body}
            return {
                "status_code": response.status,
                "headers": dict(response.headers),
                "body": body,
                "request": {
                    "url": url,
                    "headers": headers,
                    "payload": payload,
                    "timestamp": timestamp,
                }
            }
    except urllib.error.HTTPError as e:
        response_body = e.read().decode("utf-8") if e.fp else ""
        try:
            body = json.loads(response_body) if response_body else {}
        except (json.JSONDecodeError, ValueError):
            body = {"raw_body": response_body}
        return {
            "status_code": e.code,
            "error": str(e),
            "body": body,
            "request": {
                "url": url,
                "headers": headers,
                "payload": payload,
                "timestamp": timestamp,
            }
        }
    except Exception as e:
        return {
            "error": str(e),
            "request": {
                "url": url,
                "headers": headers,
                "payload": payload,
                "timestamp": timestamp,
            }
        }


def main():
    parser = argparse.ArgumentParser(description="PalmAI OpenAPI 接口调用工具")
    parser.add_argument("--secret-id", required=True, help="SecretId")
    parser.add_argument("--private-key-file", required=True, help="私钥PEM文件路径")
    parser.add_argument("--host", required=True, help="接口域名")
    parser.add_argument("--uri", required=True, help="接口URI")
    parser.add_argument("--payload", required=True, help="请求体JSON")
    parser.add_argument("--encrypt-key-file", default=None, help="加密公钥PEM文件路径")
    parser.add_argument("--encrypt-fields", default=None, help="需加密的字段（逗号分隔）")
    parser.add_argument("--output-curl", action="store_true", help="仅输出curl命令不实际调用")
    parser.add_argument("--verbose", "-v", action="store_true", help="显示完整请求详情（含签名和payload原文）")

    args = parser.parse_args()

    try:
        with open(args.private_key_file, "r", encoding="utf-8") as f:
            private_key_pem = f.read()
    except FileNotFoundError:
        print(f"错误: 私钥文件不存在: {args.private_key_file}")
        sys.exit(1)
    except PermissionError:
        print(f"错误: 无权限读取私钥文件: {args.private_key_file}")
        sys.exit(1)
    except OSError as e:
        print(f"错误: 读取私钥文件失败: {args.private_key_file} ({e})")
        sys.exit(1)

    encrypt_key_pem = None
    if args.encrypt_key_file:
        try:
            with open(args.encrypt_key_file, "r", encoding="utf-8") as f:
                encrypt_key_pem = f.read()
        except FileNotFoundError:
            print(f"错误: 加密公钥文件不存在: {args.encrypt_key_file}")
            sys.exit(1)
        except PermissionError:
            print(f"错误: 无权限读取加密公钥文件: {args.encrypt_key_file}")
            sys.exit(1)
        except OSError as e:
            print(f"错误: 读取加密公钥文件失败: {args.encrypt_key_file} ({e})")
            sys.exit(1)

    encrypt_fields = args.encrypt_fields.split(",") if args.encrypt_fields else None

    if args.output_curl:
        # 仅输出curl命令
        payload = args.payload
        if encrypt_key_pem and encrypt_fields:
            payload = encrypt_payload_fields(payload, encrypt_key_pem, encrypt_fields)

        timestamp = str(int(time.time()))
        authorization = gen_authorization(args.secret_id, private_key_pem, args.host, args.uri, payload, timestamp)

        # 默认对 Authorization 掩码，--verbose 时输出完整值
        auth_display = authorization if args.verbose else (authorization[:20] + "...***" if len(authorization) > 20 else authorization)

        payload_escaped = payload.replace("'", "'\\''")
        print(f"curl -X POST 'https://{args.host}{args.uri}' \\")
        print(f"  -H 'Authorization: {auth_display}' \\")
        print(f"  -H 'Content-Type: application/json' \\")
        print(f"  -H 'Host: {args.host}' \\")
        print(f"  -H 'X-TC-Timestamp: {timestamp}' \\")
        print(f"  -d '{payload_escaped}'")
        return

    result = call_api(
        args.secret_id, private_key_pem, args.host, args.uri,
        args.payload, encrypt_key_pem, encrypt_fields
    )

    # 默认对敏感信息掩码，--verbose 时输出完整信息
    if not args.verbose and "request" in result:
        req = result["request"]
        if "headers" in req and "Authorization" in req["headers"]:
            auth = req["headers"]["Authorization"]
            req["headers"]["Authorization"] = auth[:20] + "...***" if len(auth) > 20 else auth

    print(json.dumps(result, indent=2, ensure_ascii=False))


if __name__ == "__main__":
    main()
