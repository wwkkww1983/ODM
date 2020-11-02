let
    http = require("http"),
    path = require("path"),
    fileServer = require("./server/fileServer"),
    ajaxServer = require("./server/ajaxServer")

// 从命令行参数获取root目录，默认是当前目录:
let root = path.resolve(process.argv[2] || '.');
console.log('Static root dir: ' + root);
// 创建服务器:
let server = http.createServer((request, response) => {

    if (request.url.startsWith("/get") || request.url.startsWith("/post")) {
        // ajax 请求
        console.log("\n--------请求服务--------")
        ajaxServer(request, response)
        console.log("--------结束--------")
    } else {
        // 正常的页面与文件服务 
        fileServer(request, response, root)
    }
});

server.listen(6144);

console.log('Server is running at http://127.0.0.1:6144/ ');