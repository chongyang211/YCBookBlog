# 签名代码示例

## Java - 开放接口签名

```java
package com.example.crypto;

import java.nio.charset.StandardCharsets;
import java.security.*;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.Base64;
import javax.xml.bind.DatatypeConverter;

public class Sign {

    private final static String SECRET_ID = ""; // 客户的 SecretId
    private final static String PRIVATE_KEY = ""; // 客户的 privateKey (PKCS#8 PEM)
    private final static String algorithm = "SHA256withRSA";

    public static String sha256Hex(String s) throws NoSuchAlgorithmException {
        MessageDigest md = MessageDigest.getInstance("SHA-256");
        byte[] d = md.digest(s.getBytes(StandardCharsets.UTF_8));
        return DatatypeConverter.printHexBinary(d).toLowerCase();
    }

    private static String genAuthorization(String host, String payload, String uri, String timestamp)
            throws NoSuchAlgorithmException, InvalidKeyException, SignatureException {
        // 步骤 1：拼接规范请求串
        String httpRequestMethod = "POST";
        String canonicalUri = uri;
        String canonicalQueryString = "";
        String canonicalHeaders = "content-type:application/json\nhost:" + host + "\n";
        String signedHeaders = "content-type;host";
        String hashedRequestPayload = sha256Hex(payload);
        String canonicalRequest = httpRequestMethod + "\n" + canonicalUri + "\n" + canonicalQueryString + "\n"
                + canonicalHeaders + "\n" + signedHeaders + "\n" + hashedRequestPayload;

        // 步骤 2：拼接待签名字符串
        String hashedCanonicalRequest = sha256Hex(canonicalRequest);
        String stringToSign = algorithm + "\n" + timestamp + "\n" + hashedCanonicalRequest;

        // 步骤 3：计算签名
        String signature = sign(stringToSign);

        // 步骤 4：拼接 Authorization
        return algorithm + " Credential=" + SECRET_ID + ", " +
                "SignedHeaders=" + signedHeaders + ", " +
                "Signature=" + signature;
    }

    private static String sign(String message)
            throws NoSuchAlgorithmException, InvalidKeyException, SignatureException {
        Signature signature = Signature.getInstance(algorithm);
        signature.initSign(loadPrivateKeyFromString(PRIVATE_KEY));
        signature.update(message.getBytes(StandardCharsets.UTF_8));
        byte[] sign = signature.sign();
        return Base64.getEncoder().encodeToString(sign);
    }

    private static PrivateKey loadPrivateKeyFromString(String keyString)
            throws NoSuchAlgorithmException, InvalidKeyException {
        try {
            keyString = keyString.replace("-----BEGIN PRIVATE KEY-----", "")
                    .replace("-----END PRIVATE KEY-----", "")
                    .replaceAll("\\s+", "");
            KeyFactory keyFactory = KeyFactory.getInstance("RSA");
            PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(Base64.getDecoder().decode(keyString));
            return keyFactory.generatePrivate(keySpec);
        } catch (Exception e) {
            throw new InvalidKeyException("Invalid Private Key", e);
        }
    }

    public static void main(String[] args) {
        try {
            String host = "open-hk.palm.tencent.com";
            String payload = "{\"user_id\":\"test-user\"}";
            String uri = "/cgi-bin/get-user-palm";
            String timestamp = String.valueOf(System.currentTimeMillis() / 1000);
            String authorization = genAuthorization(host, payload, uri, timestamp);
            System.out.println("Authorization: " + authorization);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
```

## Go - 开放接口签名

```go
package main

import (
    "crypto"
    "crypto/rand"
    "crypto/rsa"
    "crypto/sha256"
    "crypto/x509"
    "encoding/base64"
    "encoding/hex"
    "encoding/pem"
    "errors"
    "fmt"
    "strconv"
    "strings"
    "time"
)

const algorithm = "SHA256withRSA"

var secretId = "Aeon-SecretId"
var privateKey = `-----BEGIN PRIVATE KEY-----
...
-----END PRIVATE KEY-----`

func main() {
    payload := `{"user_id":"test-user"}`
    host := "open-hk.palm.tencent.com"
    canonicalUri := "/cgi-bin/get-user-palm"
    timestamp := strconv.FormatInt(time.Now().Unix(), 10)
    authorization := genAuthorization(host, payload, canonicalUri, timestamp)
    fmt.Printf("Authorization: %s\n", authorization)
}

func genAuthorization(host, payload, canonicalUri, timestamp string) string {
    // 步骤 1: 拼接规范请求串
    httpRequestMethod := "POST"
    canonicalQueryString := ""
    canonicalHeaders := fmt.Sprintf("content-type:application/json\nhost:%s\n", host)
    signedHeaders := "content-type;host"
    hashedRequestPayload := sha256hex(payload)
    canonicalRequest := fmt.Sprintf("%s\n%s\n%s\n%s\n%s\n%s",
        httpRequestMethod, canonicalUri, canonicalQueryString,
        canonicalHeaders, signedHeaders, hashedRequestPayload)

    // 步骤 2: 拼接待签名字符串
    hashedCanonicalRequest := sha256hex(canonicalRequest)
    stringToSign := fmt.Sprintf("%s\n%s\n%s", algorithm, timestamp, hashedCanonicalRequest)

    // 步骤 3: 计算签名
    signature, err := RsaSignBase64withSha256([]byte(privateKey), []byte(stringToSign))
    if err != nil {
        fmt.Println("Sign error:", err)
        return ""
    }

    // 步骤 4: 拼接 Authorization
    return fmt.Sprintf("%s Credential=%s, SignedHeaders=%s, Signature=%s",
        algorithm, secretId, signedHeaders, signature)
}

func sha256hex(s string) string {
    hash := sha256.Sum256([]byte(s))
    return strings.ToLower(hex.EncodeToString(hash[:]))
}

func RsaSignBase64withSha256(privateKey []byte, data []byte) (string, error) {
    pri, err := ParsePrivateKey(privateKey)
    if err != nil {
        return "", err
    }
    hashed := sha256.Sum256(data)
    sign, err := rsa.SignPKCS1v15(rand.Reader, pri, crypto.SHA256, hashed[:])
    if err != nil {
        return "", err
    }
    return base64.StdEncoding.EncodeToString(sign), nil
}

func ParsePrivateKey(privateKey []byte) (*rsa.PrivateKey, error) {
    block, _ := pem.Decode(privateKey)
    if block == nil {
        return nil, errors.New("privatekey error")
    }
    private, err := x509.ParsePKCS8PrivateKey(block.Bytes)
    if err != nil {
        return nil, err
    }
    return private.(*rsa.PrivateKey), nil
}
```

## Python - 开放接口签名

```python
import hashlib
import time
import base64
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding

SECRET_ID = ""  # 客户的 SecretId
PRIVATE_KEY_PEM = ""  # 客户的 privateKey (PKCS#8 PEM格式)
ALGORITHM = "SHA256withRSA"

def sha256_hex(s: str) -> str:
    return hashlib.sha256(s.encode("utf-8")).hexdigest()

def sign(message: str, private_key_pem: str) -> str:
    private_key = serialization.load_pem_private_key(
        private_key_pem.encode(), password=None
    )
    signature = private_key.sign(
        message.encode("utf-8"),
        padding.PKCS1v15(),
        hashes.SHA256()
    )
    return base64.b64encode(signature).decode()

def gen_authorization(host: str, payload: str, uri: str, timestamp: str) -> str:
    # 步骤 1：拼接规范请求串
    http_request_method = "POST"
    canonical_query_string = ""
    canonical_headers = f"content-type:application/json\nhost:{host}\n"
    signed_headers = "content-type;host"
    hashed_request_payload = sha256_hex(payload)
    canonical_request = (
        f"{http_request_method}\n{uri}\n{canonical_query_string}\n"
        f"{canonical_headers}\n{signed_headers}\n{hashed_request_payload}"
    )

    # 步骤 2：拼接待签名字符串
    hashed_canonical_request = sha256_hex(canonical_request)
    string_to_sign = f"{ALGORITHM}\n{timestamp}\n{hashed_canonical_request}"

    # 步骤 3：计算签名
    signature = sign(string_to_sign, PRIVATE_KEY_PEM)

    # 步骤 4：拼接 Authorization
    return f"{ALGORITHM} Credential={SECRET_ID}, SignedHeaders={signed_headers}, Signature={signature}"

if __name__ == "__main__":
    host = "open-hk.palm.tencent.com"
    payload = '{"user_id":"test-user"}'
    uri = "/cgi-bin/get-user-palm"
    timestamp = str(int(time.time()))
    authorization = gen_authorization(host, payload, uri, timestamp)
    print(f"Authorization: {authorization}")
```

## PHP - 开放接口签名

```php
<?php

function CallOpenApi()
{
    $secretId = ""; // 客户的 secretId
    $privateKey = <<<TEXT
-----BEGIN PRIVATE KEY-----
...
-----END PRIVATE KEY-----
TEXT;

    $timestamp = time();
    $payload = '{"user_id":"test-user"}';
    $host = 'open-hk.palm.tencent.com';
    $canonicalUri = '/cgi-bin/get-user-palm';

    $authorization = genAuthorization($host, $payload, $canonicalUri, $timestamp, $secretId, $privateKey);

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "https://" . $host . $canonicalUri);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $payload);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array(
        "Authorization: " . $authorization,
        "Content-Type: application/json",
        "Host: " . $host,
        "X-TC-Timestamp: " . $timestamp,
    ));

    $result = curl_exec($ch);
    curl_close($ch);
    var_dump($result);
}

function genAuthorization($host, $payload, $canonicalUri, $timestamp, $secretId, $privateKey)
{
    $httpRequestMethod = "POST";
    $algorithm = "SHA256withRSA";
    $canonicalQueryString = "";
    $canonicalHeaders = "content-type:application/json\nhost:" . $host . "\n";
    $signedHeaders = "content-type;host";

    // step 1: build canonical request
    $hashedRequestPayload = hash("SHA256", $payload);
    $canonicalRequest = $httpRequestMethod . "\n"
        . $canonicalUri . "\n"
        . $canonicalQueryString . "\n"
        . $canonicalHeaders . "\n"
        . $signedHeaders . "\n"
        . $hashedRequestPayload;

    // step 2: build string to sign
    $hashedCanonicalRequest = hash("SHA256", $canonicalRequest);
    $stringToSign = $algorithm . "\n" . $timestamp . "\n" . $hashedCanonicalRequest;

    // step 3: sign string
    $pk = openssl_pkey_get_private($privateKey);
    $signature = "";
    openssl_sign($stringToSign, $signature, $pk, "sha256");
    $signature = base64_encode($signature);

    // step 4: build authorization
    return $algorithm
        . " Credential=" . $secretId
        . ", SignedHeaders=" . $signedHeaders
        . ", Signature=" . $signature;
}
```

## Node.js - 开放接口签名

```javascript
const crypto = require('crypto');
const https = require('https');

const SECRET_ID = ''; // 客户的 SecretId
const PRIVATE_KEY = ''; // 客户的 privateKey (PKCS#8 PEM格式)
const ALGORITHM = 'SHA256withRSA';

function sha256Hex(str) {
    return crypto.createHash('sha256').update(str, 'utf8').digest('hex');
}

function rsaSign(message, privateKey) {
    const sign = crypto.createSign('RSA-SHA256');
    sign.update(message);
    return sign.sign(privateKey, 'base64');
}

function genAuthorization(host, payload, uri, timestamp) {
    // 步骤 1：拼接规范请求串
    const httpRequestMethod = 'POST';
    const canonicalQueryString = '';
    const canonicalHeaders = `content-type:application/json\nhost:${host}\n`;
    const signedHeaders = 'content-type;host';
    const hashedRequestPayload = sha256Hex(payload);
    const canonicalRequest = [
        httpRequestMethod, uri, canonicalQueryString,
        canonicalHeaders, signedHeaders, hashedRequestPayload
    ].join('\n');

    // 步骤 2：拼接待签名字符串
    const hashedCanonicalRequest = sha256Hex(canonicalRequest);
    const stringToSign = `${ALGORITHM}\n${timestamp}\n${hashedCanonicalRequest}`;

    // 步骤 3：计算签名
    const signature = rsaSign(stringToSign, PRIVATE_KEY);

    // 步骤 4：拼接 Authorization
    return `${ALGORITHM} Credential=${SECRET_ID}, SignedHeaders=${signedHeaders}, Signature=${signature}`;
}

// 使用示例
const host = 'open-hk.palm.tencent.com';
const payload = JSON.stringify({ user_id: 'test-user' });
const uri = '/cgi-bin/get-user-palm';
const timestamp = Math.floor(Date.now() / 1000).toString();
const authorization = genAuthorization(host, payload, uri, timestamp);
console.log('Authorization:', authorization);
```

---

## Java - RSA-OAEP 加密

```java
import javax.crypto.Cipher;
import java.nio.charset.StandardCharsets;
import java.security.*;
import java.security.spec.MGF1ParameterSpec;
import java.security.spec.X509EncodedKeySpec;
import java.util.Base64;
import javax.crypto.spec.OAEPParameterSpec;
import javax.crypto.spec.PSource;

public class RSAUtil {
    private static final String RSA_OAEP_ALGORITHM = "RSA/ECB/OAEPWithSHA-256AndMGF1Padding";

    public static String encryptRSA(String plainText, PublicKey publicKey) throws Exception {
        OAEPParameterSpec oaepParams = new OAEPParameterSpec(
                "SHA-256", "MGF1", MGF1ParameterSpec.SHA256, PSource.PSpecified.DEFAULT);
        Cipher cipher = Cipher.getInstance(RSA_OAEP_ALGORITHM);
        cipher.init(Cipher.ENCRYPT_MODE, publicKey, oaepParams);
        byte[] encryptedBytes = cipher.doFinal(plainText.getBytes(StandardCharsets.UTF_8));
        return Base64.getEncoder().encodeToString(encryptedBytes);
    }

    public static PublicKey parsePublicKey(String publicKeyPem) throws Exception {
        String publicKeyBase64 = publicKeyPem
                .replace("-----BEGIN PUBLIC KEY-----", "")
                .replace("-----END PUBLIC KEY-----", "")
                .replaceAll("\\s+", "");
        byte[] keyBytes = Base64.getDecoder().decode(publicKeyBase64);
        X509EncodedKeySpec keySpec = new X509EncodedKeySpec(keyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        return keyFactory.generatePublic(keySpec);
    }
}
```

---

## Java - 推送验签

```java
import java.nio.charset.StandardCharsets;
import java.security.*;
import java.security.spec.X509EncodedKeySpec;
import java.util.Base64;
import java.util.regex.*;

public class SignatureVerifier {
    private static final String ALGORITHM = "SHA256withRSA";
    private static final Pattern SIGNATURE_PATTERN = Pattern.compile("Signature=([^,]+)");

    public static boolean verifyPushSignature(String method, String uri, String query,
            String contentType, String host, String body,
            String timestamp, String signatureBase64, String publicKeyPem) throws Exception {
        // 1. 拼接规范请求串
        String canonicalHeaders = String.format("content-type:%s\nhost:%s\n", contentType, host);
        String signedHeaders = "content-type;host";
        String hashedPayload = sha256Hex(body);
        String canonicalRequest = String.format("%s\n%s\n%s\n%s\n%s\n%s",
                method, uri, query, canonicalHeaders, signedHeaders, hashedPayload);

        // 2. 拼接待签名字符串
        String hashedCanonicalRequest = sha256Hex(canonicalRequest);
        String stringToSign = String.format("%s\n%s\n%s", ALGORITHM, timestamp, hashedCanonicalRequest);

        // 3. 验证签名
        PublicKey publicKey = parsePublicKey(publicKeyPem);
        byte[] signatureBytes = Base64.getDecoder().decode(signatureBase64);
        Signature sig = Signature.getInstance("SHA256withRSA");
        sig.initVerify(publicKey);
        sig.update(stringToSign.getBytes(StandardCharsets.UTF_8));
        return sig.verify(signatureBytes);
    }

    private static String sha256Hex(String input) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] hash = digest.digest(input.getBytes(StandardCharsets.UTF_8));
            StringBuilder hexString = new StringBuilder();
            for (byte b : hash) {
                String hex = Integer.toHexString(0xff & b);
                if (hex.length() == 1) hexString.append('0');
                hexString.append(hex);
            }
            return hexString.toString();
        } catch (Exception e) {
            throw new RuntimeException("SHA256 calculation failed", e);
        }
    }

    private static PublicKey parsePublicKey(String publicKeyPem) throws Exception {
        String publicKeyContent = publicKeyPem
                .replace("-----BEGIN PUBLIC KEY-----", "")
                .replace("-----END PUBLIC KEY-----", "")
                .replaceAll("\\s", "");
        byte[] keyBytes = Base64.getDecoder().decode(publicKeyContent);
        X509EncodedKeySpec keySpec = new X509EncodedKeySpec(keyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        return keyFactory.generatePublic(keySpec);
    }

    public static String extractSignature(String authorization) {
        Matcher matcher = SIGNATURE_PATTERN.matcher(authorization);
        return matcher.find() ? matcher.group(1) : null;
    }
}
```
