const http = require('http');
const URL = require('url');
const url = URL.URL;
const crypto = require('crypto');
const {WebSocket,WebSocketServer} = require('ws')
let nodeMcu = [];
let otherClient = [];
const {StringDecoder} = require('string_decoder');
const server = http.createServer((req,res)=>{
    //getting headers in request 
    const headers = req.headers;
    //getting the url
    const myurl = new url(req.url,`http://${req.headers.host}`);

    //getting the method
    const method = req.method.toUpperCase();

    //getting the path and removing / from end of the path 
    const path = myurl.pathname.replace(/\/$|\/+\/$/g," ")

    //getting the query in url 
    const query = myurl.searchParams;

    //getting the payload
    const decoder =new  StringDecoder('utf-8');
    let buffer = '';
    req.on('data',(data)=>{
        buffer += decoder.write(data);
    })
    req.on('end',()=>{
        buffer += decoder.end();
        //ending the request
        res.writeHead(200,
            "IOT appliance controller",
            {
            'content-type':'application/json'
        })
        res.end(JSON.stringify({
            data:"MSS303"
        }));
    })
})
server.listen(3000,()=>{console.log("server is listening")})

const wss = new WebSocketServer({
  clientTracking:true,
  server:server
})
 const testData={
   msg:"MSG20267"
 }
wss.on('connection',(ws,req)=>{
  ws.on('message',(data,isbinary)=>{
    if(data.toString() === "MSG201"){//esp8266
      wss.clients.forEach(client=>{
        if(client == ws && client.readyState === WebSocket.OPEN){
          nodeMcu.push(client);
        }
      })
    }else if(data.toString() === "MSG202"){//other devices
      wss.clients.forEach(client=>{
        if(client == ws && client.readyState === WebSocket.OPEN){
          otherClient.push(client);
        }
      })
    }
    var flag ;
    try{
      JSON.parse(data.toString());
      flag = true;

    }catch{
       flag= false;
    }
    console.log(flag);
    if(flag){
      const temp = JSON.parse(data.toString());
      console.log(temp);
      if(temp.dest === "MSG201"){
        const esp8266DATA = temp.id + temp.state;
        console.log(esp8266DATA)
        nodeMcu.forEach(client=>{
          client.send(esp8266DATA,{binary:isbinary})
        })
      }else if(temp.dest === "MSG202"){
        const compDATA = temp.id + temp.state;
        console.log(compDATA)
        otherClient.forEach(client=>{
          client.send(compDATA,{binary:isbinary})
        })
      }
    }
    // wss.clients.forEach((client)=>{
    //   if(client !== ws && client.readyState === WebSocket.OPEN){
    //     client.send(data,{binary:isbinary});
    //     console.log(data.toString());
    //   }
    // })
  })
  ws.send(JSON.stringify(testData));
  const ip = req.socket.remoteAddress;
  console.log("ip address",ip);
})