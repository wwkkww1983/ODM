let createLicense = require("../createLicense/createLicense")

let ajaxServer = (request, response) => {
    console.log("url=" + request.url)
    response.writeHead(200)
    // response.writeHead(200, "ok", {
    //     "Content-type": "application/x-javascript",
    //     "Access-Control-Allow-Origin": "http://localhost:6144",
    //     "Access-Control-Allow-Headers": "damu",
    //     "Access-Control-Allow-Methods": "PUT,DELETE,POST",
    //     "Access-Control-Max-Age": "10"
    // })

    // 解析 url
    let info = {}
    let form = request.url.split("?")
    form = form[1].split("&")

    // 序列化信息
    form.map((item) => {
        let newItem = item.split("=")
        info[newItem[0]] = newItem[1]
    })

    let License = createLicense(info.id, info.date)
    response.end(License)
}


module.exports = ajaxServer