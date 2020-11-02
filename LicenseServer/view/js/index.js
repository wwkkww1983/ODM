/**
 * 向后端发送请求，并获取返回文本或状态,执行 handler 中的回调函数
 * 可以传入自定义的处理函数对象AjaxHandler，默认行为是打印
 * @param {String} type 
 * @param {String} url 
 * @param {Object} AjaxHandler 
 * 
 */
let Ajax = (type, url, AjaxHandler) => {
    AjaxHandler = AjaxHandler || {
        success: (text) => {
            console.log(text)
        },
        fail: (code) => {
            console.log(code)
        }
    }
    let request = new XMLHttpRequest(); // 新建XMLHttpRequest对象
    request.onreadystatechange = () => { // 状态发生变化时，函数被回调
        if (request.readyState === 4) { // 成功完成
            // 判断响应结果:
            if (request.status === 200) {
                AjaxHandler.success(request.responseText);
            } else {
                // 失败，根据响应码判断失败原因:
                AjaxHandler.fail(request.status);
            }
        } else {
            // HTTP请求还在继续...
        }
    }

    // 发送请求:
    request.open(type, url);
    request.send();

    return request.onreadystatechange();
}


/**
 * 解密
 * @param {String} pwd 
 */
let deciphering = (pwd) => {

    let id = ""
    let date = ""
    let licNum = 0
    console.log(pwd)
    pwd = pwd.split("-")

    // 电路板序列号解密测试
    for (let i = 0; i < 3; i++) {
        id += (pwd[i][0].charCodeAt() - pwd[i][1].charCodeAt() - 1)
        id += (pwd[i][2].charCodeAt() - pwd[i][3].charCodeAt() - 1)
    }
    console.log("打印电路板序号")
    console.log(id)

    // 使用时长解密
    date = (pwd[3][0].charCodeAt() - 65 - pwd[1][1].charCodeAt() % 6) * 20 + (pwd[3][1].charCodeAt() - 65 - pwd[1][3].charCodeAt() % 6)
    console.log("打印使用时长")
    console.log(date)

    // 密钥编号解密
    licNum = (pwd[3][2].charCodeAt() - 65 - pwd[0][1].charCodeAt() % 10) * 16 + (pwd[3][3].charCodeAt() - 65 - pwd[0][3].charCodeAt() % 10)
    console.log("打印密钥编号")
    console.log(licNum)

    if (date == 0) {
        console.log("您已获得永久使用权")
    }
}

/**
 * odm序列号及输入格式校验
 * @param {String} odm_id 
 * @param {String} odm_date 
 */
let checkInfo = (odm_id, odm_date) => {
    // 验证序列号格式
    let id = odm_id.split("-")
    let date = odm_date
    if (id[0] !== "ODM" || id[1].length !== 1 || id[2].length !== 5 || id[3].length !== 6 || isNaN(id[3])) {
        alert("序列号格式错误")
        return false;
    }

    // 验证日期，最长 365 天，不足 3 位数的用 0 补齐
    // 0 为永久许可命令
    if (date > 365) {
        alert("单个限时许可最长为一年（365）")
        return false;
    } else if (date >= 100) {
    } else if (date >= 10) {
        date = "0" + date;
    } else if (date >= 0) {
        date = "00" + date;
    } else {
        alert("日期不能为空值或负数");
        return false;
    }

    return true
}

let main = () => {
    let odm_id = document.querySelector("#odm-id");
    let odm_date = document.querySelector("#odm-date");
    let odm_pwd = document.querySelector("#odm-pwd");
    let btn_createPwd = document.querySelector("#btn-createPwd");

    // 校验信息并发送请求，获取该 ODM 对应的 license
    btn_createPwd.onclick = () => {
        if (!checkInfo(odm_id.value, odm_date.value)) {
            return;
        }

        let url = "/getPwd?id=" + odm_id.value + "&date=" + odm_date.value

        // 定义请求的成功与失败函数，参数为返回的内容
        let AjaxHandler = {
            success: (text) => {
                odm_pwd.value = text
                console.log("------解密测试------")
                deciphering(text)
            },
            fail: (code) => {
                console.log("Error code: " + code)
            }

        }

        // 向服务器发送请求，成功则执行 success，失败执行 fail
        Ajax('GET', url, AjaxHandler)
    }
}

document.addEventListener("DOMContentLoaded", main, false);



