# 自有平台web端接口说明

## 自有平台web接口地址

| 环境 | http | https |
| - | - | - |
| 测试 | http://192.168.1.9:8080/v1/server/ | 无
| 正式 | http://api.laywoodgames.com//v1/server/ | https://api.laywoodgames.com//v1/server/ |

## web接口的加密方式

### POST接口

所有参数均通过content data(json格式) 并 需要加密后的签名

示例:

```html
//获取新的激活码
get_activeation_code?timestamp=1540362585&appid=1&sig=xxxxxxx
method: post
```

http_body

```json
{
    "user_id" : "11111111111",
    "channel" : "1"
}
```

?> sig = md5(appkey + timestamp) //平台可以通过appid获取到appkey, 游戏内可以通过配置拿到

### GET接口

所有的参数均通过url参数的方式传入 并 需要加密后的签名

示例:

```html
interface?timestamp=1540362585&appid=1&sig=xxxxxxx
```

?> sig = md5(appkey + timestamp) //平台可以通过appid获取到appkey, 游戏内可以通过配置拿到

!> 为防止部分机器的时间有误差, 时间戳(timestamp)有容忍区间, 可由平台指定(例如10分钟), 当client请求接口时间在平台服务器的容忍时间区间内，则依然可以正常服务, 否则返回对应的response，response中附带对应的错误码

## 所有的web接口及示例说明

### 获取激活码

- 接口名: getActivationCode
- 接口说明: 获取游戏激活码
- URL格式: http://192.168.1.9:8080/v1/server/getActivationCode?sig=dsafatewr&appid=10001&timestamp=1578745715
- 入参说明:

| 参数名称 | 类型 | 描述 |
| - | - | - |
| accountid | uint32 | 游戏内角色id |
| channel | int | 渠道id |

- 返回说明: 返回结果格式为json, 字段名说明为

| 参数名称 | 类型 | 描述 |
| - | - | - |
| code | int | 返回码, 0:正确 其他:失败 |
| msg | string | code非0，则表示"错误码, 错误提示", 详细注释参见错误码描述 |

- 示例:

```html
POST v1/server/getActivationCode?sig=dsafatewr&appid=10001&timestamp=1578745715
HTTP/1.0
Host:192.168.1.9:8080
Content-Type: application/x-www-form-urlencoded
Content-Length: 100

{
    "accountid": 11111,
    "channel": 1
}

//返回结果:
{"code":0,"msg":""}
```

### 使用激活码

- 接口名: verifyActivationCode
- 接口说明: 使用激活码
- URL格式: http://192.168.1.9:8080/v1/server/verifyActivationCode?sig=dsafatewr&appid=10001&timestamp=1578745715
- 入参说明:

| 参数名称 | 类型 | 描述 |
| - | - | - |
| openid | string | 渠道账号id |
| accountid | uint32 | 游戏内角色id |
| channel | int | 渠道id |
| activationCode | string | 激活码 |

- 返回说明: 返回结果格式为json, 字段名说明为

| 参数名称 | 类型 | 描述 |
| - | - | - |
| code | int | 返回码, 0:正确 其他:失败 |
| msg | string | code非0，则表示"错误码, 错误提示", 详细注释参见错误码描述 |
| data | json | 返回的数据, "activationCode":"激活码" |

- 示例:

```html
POST v1/server/verifyActivationCode?sig=dsafatewr&appid=10001&timestamp=1578745715
HTTP/1.0
Host:192.168.1.9:8080
Content-Type: application/x-www-form-urlencoded
Content-Length: 100

{
    "openid": "xxxxxxxx",
    "accountid": 11111,
    "channel": 1,
    "activationCode": "xxxxxxxxxxxxx"
}

//返回结果:
{"code":0,"msg":"Request is successful","data":{"activationCode":"激活码"}}
```

### 邀请码上报

- 接口名: invitationCode
- 接口说明: 获取游戏激活码
- URL格式: http://192.168.1.9:8080/v1/server/invitationCode?sig=dsafatewr&appid=10001&timestamp=1578745715
- 入参说明:

| 参数名称 | 类型 | 描述 |
| - | - | - |
| accountid | uint32 | 游戏内角色id |
| channel | int | 渠道id |
| inviterid | uint32 | 邀请人游戏内id |

- 返回说明: 返回结果格式为json, 字段名说明为

| 参数名称 | 类型 | 描述 |
| - | - | - |
| code | int | 返回码, 0:正确 其他:失败 |
| msg | string | code非0，则表示"错误码, 错误提示", 详细注释参见错误码描述 |
| data | json | 返回的数据 |

- 示例:

```html
POST v1/server/invitationCode?sig=dsafatewr&appid=10001&timestamp=1578745715
HTTP/1.0
Host:192.168.1.9:8080
Content-Type: application/x-www-form-urlencoded
Content-Length: 100

{
    "accountid": 11111,
    "channel": 1,
    "inviterid": 22222
}

//返回结果:
{"code":0,"msg":"I have already added","data":""}
```