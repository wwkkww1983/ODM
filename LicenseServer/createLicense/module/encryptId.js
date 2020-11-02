/**
 * 加密，将数字加上随机数，变为大写字母
 * @param {String} str
 * @param {Number} interval
 * @param {Number} range
 *
 * str 表示要加密的字符串
 * interval 表示每加密几个字符，就更换随机数
 * range 表示随机数的范围
 */
let encryptId = (str, interval, range) => {
    let result = "";
    let random = Math.round(range * Math.random());
    let tmp = ""

    //console.log(str,interval,random)
    if (str.length == 0 || interval == 0) {
        return []
    }

    // 整个模式为，转换后的字符串 str + random 的模式，即每个周期后就将随机数添加到字符串后面
    for (let i = 0; i < str.length; i++) {
        if (i % interval == 0 && i >= interval) {
            tmp += String.fromCharCode(65 + random);
            result += tmp;
            tmp = "";
            random = Math.round(range * Math.random());
        }
        tmp += String.fromCharCode(65 + parseInt(str[i]) + random + 1)
    }
    tmp += String.fromCharCode(65 + random);
    result += tmp;
    return result;
}

module.exports = encryptId