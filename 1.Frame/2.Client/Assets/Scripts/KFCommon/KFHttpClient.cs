using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using Newtonsoft.Json.Linq;

public class KFHttpClient
{
    public static string Request(string url, string data)
    {
        HttpWebRequest request = WebRequest.Create(url) as HttpWebRequest;
        CookieContainer cookieContainer = new CookieContainer();
        request.CookieContainer = cookieContainer;
        request.AllowAutoRedirect = true;
        //request.AllowReadStreamBuffering = true;
        request.MaximumResponseHeadersLength = 1024;
        request.Method = "POST";
        request.AllowAutoRedirect = true;
        request.MaximumResponseHeadersLength = 1024;
        request.ContentType = "application/json";

        byte[] databyte = Encoding.UTF8.GetBytes(data);
        Stream postStream = request.GetRequestStream();
        postStream.Write(databyte, 0, databyte.Length);
        postStream.Close();

        HttpWebResponse res;
        try
        {
            res = (HttpWebResponse)request.GetResponse();
        }
        catch (WebException ex)
        {
            res = (HttpWebResponse)ex.Response;
            return "";
        }

        StreamReader sr = new StreamReader(res.GetResponseStream(), Encoding.UTF8);
        string content = sr.ReadToEnd();
        return content;
    }

    public static JObject Request(string url, JObject json)
    {
        // 转换成字串
        string data = json.ToString();
        data = Request(url, data);
        var result = JObject.Parse(data);
        return result;
    }
}
