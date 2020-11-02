let fs = require("fs")

let getHistory = (path_licDir, path_licFile) => {
    let licNum = 0;

    if (fs.existsSync(path_licDir)) {
        if (fs.existsSync(path_licFile)) {
            console.log("打印历史数据")
            let text = fs.readFileSync(path_licFile, 'utf8');
            console.log(text)
            console.log("历史数据读取完毕")
            licNum = text.split("\n").length

        } else {
            licNum = 1;
        }
    } else {
        console.log("不存在历史数据，创建")
        fs.mkdirSync(path_licDir);
        licNum = 1;
    }
    return licNum
}

module.exports = getHistory