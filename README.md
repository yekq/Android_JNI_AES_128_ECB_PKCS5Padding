# Android_JNI_AES_128_ECB_PKCS5Padding
支持不定长数据的加解密,测试过与PHP的结果互通

AES_128_ECB From 
[https://github.com/kokke/tiny-AES128-C](https://github.com/kokke/tiny-AES128-C)

## 新增加不定长字符的加密/解密方法:

```c
char* AES_128_ECB_PKCS5Padding_Encrypt(const char *in,const uint8_t *key);
char* AES_128_ECB_PKCS5Padding_Decrypt(const char *in, const uint8_t* key);
```

## Demo里面的key是:
```c
const uint8_t AES_KEY[]="1234567890abcdef";
```

## 例子:
```c

加密前:1  

加密后:qkrxxA9fIF636aITDRJhcg==  

```

```c

加密数据:
Yrl8Sryq7Kpce4UWRfG3bBBYpzXv59Muj0wjkJYRHFb73CogeD
RfQCXsjSfxTe0gibaf+f1FLekwow0f1W9stJy3q7CNOPzkSJVdCtyZvI
xMxLwz9hyatUJnU4Nq6i2gkaiCZcwHuDtrAHpEoy1k0vudpWhGu2457i
Sc40Tqw4tQnxKX18DcKNG5/KPUM+A5Y9a3FxaAy84Turio78b+6A==

解密结果:
{"Json解析":"支持格式化高亮折叠","支持XML转换":"支持XML转换Json,Json转XML","Json格式验证":"更详细准确的错误信息"}

```


## 在线校验的网站:
[http://www.seacha.com/tools/aes.html](http://www.seacha.com/tools/aes.html)

## Demo截图:
![](https://github.com/yekq/Android_JNI_AES_128_ECB_PKCS5Padding/blob/master/device-2016-11-09-174640.png)
