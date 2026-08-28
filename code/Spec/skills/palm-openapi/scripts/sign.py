#!/usr/bin/env python3
"""
PalmAI OpenAPI 签名计算工具

用法:
    python3 sign.py sign --secret-id <SECRET_ID> --private-key-file <PEM_FILE> \
                         --host <HOST> --uri <URI> --payload <JSON_BODY> \
                         [--timestamp <TIMESTAMP>] [--verbose]

    python3 sign.py verify --public-key-file <PEM_FILE> --host <HOST> \
                           --uri <URI> --body <BODY> --timestamp <TS> \
                           --signature <BASE64_SIG> [--verbose]

示例:
    # 生成签名
    python3 sign.py sign --secret-id "Aeon-SecretId" \
                         --private-key-file private-key.pem \
                         --host "open-hk.palm.tencent.com" \
                         --uri "/cgi-bin/get-user-palm" \
                         --payload '{"user_id":"test-user"}' \
                         --verbose

    # 验证推送签名
    python3 sign.py verify --public-key-file public-key.pem \
                           --host "open-hk.palm.tencent.com" \
                           --uri "/api/callback" \
                           --body '{"action":"update_palm"}' \
                           --timestamp "1700000000" \
                           --signature "Base64SignatureString"
"""

import argparse
import hashlib
import time
import base64
import sys
import json

try:
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import padding as asym_padding
except ImportError:
    print("错误: 需要安装 cryptography 库")
    print("运行: pip3 install cryptography")
    sys.exit(1)

ALGORITHM = "SHA256withRSA"


def sha256_hex(s: str) -> str:
    """计算字符串的SHA256哈希值（小写十六进制）"""
    return hashlib.sha256(s.encode("utf-8")).hexdigest()


def rsa_sign(message: str, private_key_pem: str) -> str:
    """使用RSA私钥对消息进行SHA256withRSA签名"""
    private_key = serialization.load_pem_private_key(
        private_key_pem.encode(), password=None
    )
    signature = private_key.sign(
        message.encode("utf-8"),
        asym_padding.PKCS1v15(),
        hashes.SHA256()
    )
    return base64.b64encode(signature).decode()


def gen_authorization(secret_id: str, private_key_pem: str,
                      host: str, uri: str, payload: str,
                      timestamp: str, verbose: bool = False) -> str:
    """生成完整的Authorization头"""

    # 步骤 1：拼接规范请求串
    http_request_method = "POST"
    canonical_query_string = ""
    canonical_headers = f"content-type:application/json\nhost:{host}\n"
    signed_headers = "content-type;host"
    hashed_request_payload = sha256_hex(payload)

    canonical_request = (
        f"{http_request_method}\n"
        f"{uri}\n"
        f"{canonical_query_string}\n"
        f"{canonical_headers}\n"
        f"{signed_headers}\n"
        f"{hashed_request_payload}"
    )

    if verbose:
        print("=" * 60)
        print("步骤 1: 规范请求串 (CanonicalRequest)")
        print("=" * 60)
        print(canonical_request)
        print(f"\nHashedRequestPayload: {hashed_request_payload}")
        print()

    # 步骤 2：拼接待签名字符串
    hashed_canonical_request = sha256_hex(canonical_request)
    string_to_sign = f"{ALGORITHM}\n{timestamp}\n{hashed_canonical_request}"

    if verbose:
        print("=" * 60)
        print("步骤 2: 待签名字符串 (StringToSign)")
        print("=" * 60)
        print(string_to_sign)
        print(f"\nHashedCanonicalRequest: {hashed_canonical_request}")
        print()

    # 步骤 3：计算签名
    signature = rsa_sign(string_to_sign, private_key_pem)

    if verbose:
        print("=" * 60)
        print("步骤 3: 签名 (Signature)")
        print("=" * 60)
        print(signature)
        print()

    # 步骤 4：拼接 Authorization
    authorization = (
        f"{ALGORITHM} Credential={secret_id}, "
        f"SignedHeaders={signed_headers}, "
        f"Signature={signature}"
    )

    if verbose:
        print("=" * 60)
        print("步骤 4: Authorization")
        print("=" * 60)
        print(authorization)
        print()
        print("=" * 60)
        print("完整请求头")
        print("=" * 60)
        print(f"Authorization: {authorization}")
        print(f"Content-Type: application/json")
        print(f"Host: {host}")
        print(f"X-TC-Timestamp: {timestamp}")
        print()
        print("=" * 60)
        print("curl 命令")
        print("=" * 60)
        payload_escaped = payload.replace("'", "'\\''")  
        # verbose 模式下 curl 命令中的 Authorization 也做掩码，避免拷贝泄露
        auth_curl = authorization[:20] + "...***" if len(authorization) > 20 else authorization
        print(f"curl -X POST 'https://{host}{uri}' \\")
        print(f"  -H 'Authorization: {auth_curl}' \\")
        print(f"  -H 'Content-Type: application/json' \\")
        print(f"  -H 'Host: {host}' \\")
        print(f"  -H 'X-TC-Timestamp: {timestamp}' \\")
        print(f"  -d '{payload_escaped}'")
        print()
        print("⚠️  curl 命令中 Authorization 已掩码，完整值见上方步骤4输出")

    return authorization


def verify_signature(public_key_pem: str, method: str, uri: str,
                     query: str, content_type: str, host: str,
                     body: str, timestamp: str, signature_b64: str,
                     verbose: bool = False) -> bool:
    """验证推送回调签名"""
    # 重建待签名字符串
    canonical_headers = f"content-type:{content_type}\nhost:{host}\n"
    signed_headers = "content-type;host"
    hashed_payload = sha256_hex(body)

    canonical_request = (
        f"{method}\n{uri}\n{query}\n"
        f"{canonical_headers}\n{signed_headers}\n{hashed_payload}"
    )

    hashed_canonical_request = sha256_hex(canonical_request)
    string_to_sign = f"{ALGORITHM}\n{timestamp}\n{hashed_canonical_request}"

    if verbose:
        print("=" * 60)
        print("验签 - 重建的待签名字符串")
        print("=" * 60)
        print(f"CanonicalRequest:\n{canonical_request}\n")
        print(f"HashedCanonicalRequest: {hashed_canonical_request}")
        print(f"StringToSign:\n{string_to_sign}\n")

    # 加载公钥并验证
    public_key = serialization.load_pem_public_key(public_key_pem.encode())
    signature_bytes = base64.b64decode(signature_b64)

    try:
        public_key.verify(
            signature_bytes,
            string_to_sign.encode("utf-8"),
            asym_padding.PKCS1v15(),
            hashes.SHA256()
        )
        if verbose:
            print("✅ 验签通过")
        return True
    except Exception as e:
        if verbose:
            print(f"❌ 验签失败: {e}")
        return False


def main():
    parser = argparse.ArgumentParser(description="PalmAI OpenAPI 签名计算工具")
    subparsers = parser.add_subparsers(dest="command", help="子命令")

    # sign 子命令
    sign_parser = subparsers.add_parser("sign", help="生成签名")
    sign_parser.add_argument("--secret-id", required=True, help="SecretId")
    sign_parser.add_argument("--private-key-file", required=True, help="私钥PEM文件路径")
    sign_parser.add_argument("--host", required=True, help="接口域名")
    sign_parser.add_argument("--uri", required=True, help="接口URI")
    sign_parser.add_argument("--payload", required=True, help="请求体JSON")
    sign_parser.add_argument("--timestamp", default=None, help="时间戳（默认当前时间）")
    sign_parser.add_argument("--verbose", "-v", action="store_true", help="显示详细中间值")

    # verify 子命令
    verify_parser = subparsers.add_parser("verify", help="验证推送签名")
    verify_parser.add_argument("--public-key-file", required=True, help="公钥PEM文件路径")
    verify_parser.add_argument("--method", default="POST", help="HTTP方法")
    verify_parser.add_argument("--uri", required=True, help="请求URI")
    verify_parser.add_argument("--query", default="", help="查询参数")
    verify_parser.add_argument("--content-type", default="application/json", help="Content-Type")
    verify_parser.add_argument("--host", required=True, help="Host")
    verify_parser.add_argument("--body", required=True, help="请求体")
    verify_parser.add_argument("--timestamp", required=True, help="X-TC-Timestamp")
    verify_parser.add_argument("--signature", required=True, help="Base64签名")
    verify_parser.add_argument("--verbose", "-v", action="store_true", help="显示详细中间值")

    args = parser.parse_args()

    if args.command == "sign":
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

        try:
            timestamp = args.timestamp or str(int(time.time()))
            authorization = gen_authorization(
                args.secret_id, private_key_pem,
                args.host, args.uri, args.payload,
                timestamp, args.verbose
            )
            if not args.verbose:
                # 默认掩码输出，与 call_api.py 保持一致；需完整值请加 --verbose
                auth_display = authorization[:20] + "...***" if len(authorization) > 20 else authorization
                print(auth_display)
                print("(默认已掩码，使用 --verbose 查看完整签名)", file=sys.stderr)
        except ValueError as e:
            print(f"错误: 私钥格式无效: {e}")
            sys.exit(1)
        except Exception as e:
            print(f"错误: 签名计算失败: {e}")
            sys.exit(1)

    elif args.command == "verify":
        try:
            with open(args.public_key_file, "r", encoding="utf-8") as f:
                public_key_pem = f.read()
        except FileNotFoundError:
            print(f"错误: 公钥文件不存在: {args.public_key_file}")
            sys.exit(1)
        except PermissionError:
            print(f"错误: 无权限读取公钥文件: {args.public_key_file}")
            sys.exit(1)
        except OSError as e:
            print(f"错误: 读取公钥文件失败: {args.public_key_file} ({e})")
            sys.exit(1)

        try:
            result = verify_signature(
                public_key_pem, args.method, args.uri,
                args.query, args.content_type, args.host,
                args.body, args.timestamp, args.signature,
                args.verbose
            )
            sys.exit(0 if result else 1)
        except ValueError as e:
            print(f"错误: 公钥格式无效: {e}")
            sys.exit(1)
        except Exception as e:
            print(f"错误: 验签过程失败: {e}")
            sys.exit(1)

    else:
        parser.print_help()


if __name__ == "__main__":
    main()
