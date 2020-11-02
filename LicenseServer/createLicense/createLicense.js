
let encryptId = require("./module/encryptId"),
    getHistory = require("./module/getHistory"),
    fs = require("fs")

/**
 * 
 * @param {String} odm_id 
 * @param {String} odm_date 
 */
let createLicense = (odm_id, odm_date) => {
    console.log("\n-----ODM 证书生成服务-----")
    console.log("id: " + odm_id)
    console.log("date: " + odm_date)

    // 拆分序列号
    // 示例 ODM-S-D1000-190706
    let id = odm_id.split("-");
    let date = odm_date;
    let pwd = []

    let path_licDir = "./createLicense/History" // 保存历史数据的文件夹 
    let path_licFile = path_licDir + "/" + odm_id + ".txt" // 每个 ODM 的许可证记录
    let licNum = getHistory(path_licDir, path_licFile); // 获取本次密钥的序号

    console.log("密钥编号: " + licNum)
    // 序列号加密
    // 用随机数加上每一个原数字，然后大写字母化
    for (let i = 0; i < 3; i++) {
        // 序列号共6位，每2位数字为1组
        // 每个数字单独用不同的随机数加密
        // 随机数范围是 0 ~ 15
        pwd[i] = encryptId(id[3].substr(2 * i, 2), 1, 15);
    }

    // 再对日期进行加密
    // 考虑到设备不联网，因此需要产生一个递增的序号，作为历史标记，避免重用
    // 前两位是日期，暂定为 20 * 20 的组合，将第二组的两个随机数，分别与 6 求余，加到序号上
    // 最后两位作为序号，暂定为 16 * 16 种可能，为了避免明显的 ABCD... 将第一组中的两个随机数，分别与 10 求余，加到序号上

    pwd[3] = "";

    date = [Math.floor(date / 20) + pwd[1][1].charCodeAt() % 6, date % 20 + pwd[1][3].charCodeAt() % 6].map((item) => {
        item = String.fromCharCode(item + 65)
        pwd[3] += item
        return item
    })

    licNum = [Math.floor(licNum / 16) + pwd[0][1].charCodeAt() % 10, licNum % 16 + pwd[0][3].charCodeAt() % 10].map((item) => {
        item = String.fromCharCode(item + 65)
        pwd[3] += item
        return item
    })

    // 返回密码
    pwd = pwd.join("-")
    console.log(pwd)

    // 将新密钥添加到历史记录
    fs.appendFile(path_licFile, pwd + '\n', 'utf8', () => {
        console.log("新密钥已加入历史记录" + path_licFile)
    })

    return pwd
}

module.exports = createLicense