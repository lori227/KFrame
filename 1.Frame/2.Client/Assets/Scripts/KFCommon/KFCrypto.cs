using System.Collections;
using System;
using System.Text;
using System.Collections.Generic;
using System.Security.Cryptography;

public class KFCrypto
{
    // 16位md5加密
    public static string MD5Encrypt16(string source)
    {
        string result = "";
        if (source != "")
        {
            var md5 = new MD5CryptoServiceProvider();
            var buffer = md5.ComputeHash(Encoding.Default.GetBytes(source));

            var temp = BitConverter.ToString(buffer, 4, 8);
            source = temp.Replace("-", "");
        }

        return result;
    }

    // 32位加密
    public static string MD5Encrypt32(string source)
    {
        string result = "";
        if (source != "")
        {
            MD5 md5 = MD5.Create();
            // 加密后是一个字节类型的数组，这里要注意编码UTF8/Unicode等的选择　
            byte[] s = md5.ComputeHash(Encoding.UTF8.GetBytes(source));

            // 通过使用循环，将字节类型的数组转换为字符串，此字符串是常规字符格式化所得
            for (int i = 0; i < s.Length; i++)
            {
                // 将得到的字符串使用十六进制类型格式。格式后的字符是小写的字母，如果使用大写（X）则格式后的字符是大写字符 
                result = result + s[i].ToString("x");
            }
        }

        return result;
    }

    // 64位加密
    public static string MD5Encrypt64(string source)
    {
        string result = "";
        if (source != "")
        {

            MD5 md5 = MD5.Create();

            // 加密后是一个字节类型的数组，这里要注意编码UTF8/Unicode等的选择　
            byte[] s = md5.ComputeHash(Encoding.UTF8.GetBytes(source));
            result = Convert.ToBase64String(s);
        }

        return result;
    }
}
