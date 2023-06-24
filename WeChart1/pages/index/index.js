// index.js
var mqtt = require('../mq/mqtt.min.js') //根据自己存放的路径修改
var client=null
Page({
  data: {
   temp:'0',
   humi:'0',
   pm:'0',
   light:'0',
   inled:false,
   outled:false
  },
  // 事件处理函数
  
  onLoad() {
    this.connectmqtt();
  },
 connectmqtt:function(){
   var that=this
   const options={
     protocolVersion: 4,
     clean: true,
     connectTimeout:4000,
     /*clientId: 'emqx_cloud_miniprogram',*/
     clientId: 'lhj'+Math.ceil(Math.random()*10),
     port:'8084/mqtt',
     username:'',
     password:'',
     reconnectPeriod:1000,
     connectTimeout:30*1000,
     resubscribe:true
   }
   client=mqtt.connect('wxs://lbsmqtt.airm2m.com',options)
   client.on('connect',(e)=>{
     console.log('服务器连接成功')
     client.subscribe('mqhome',{qos:0},function(err){
       if(!err)
       {console.log('订阅成功')}
     } )
   })
   //信息监听
    client.on('message', function (topic, message) {
      let tem={}
      tem=JSON.parse(message)
      that.setData({
        temp:tem.temp,
        humi:tem.humi,
        pm:tem.pm
      })
      console.log(tem)
      console.log('received msg:' + message.toString());
      })
  },
  inledchange:function(event){
    console.log(event.detail)
    let inled=event.detail.value
    console.log(event.detail.value)
    if(inled)
    {
      client.publish('LED', 'inled:1',function(err){
        if(!err)
        {
          console.log('inled发送指令开');
        }
      })
    }
    else
    {
      client.publish('LED', 'inled:0',function(err){
        if(!err)
        {
          console.log('inled发送指令关');
        }
      })
    }
  },

  outledchange:function(event){
    console.log(event.detail)
    let outled=event.detail.value
    console.log(event.detail.value)
    if(outled)
    {
      client.publish('LED', 'outled:1',function(err){
        if(!err)
        {
          console.log('outled发送指令开');
        }
      })
    }
    else
    {
      client.publish('LED', 'outled:0',function(err){
        if(!err)
        {
          console.log('outled发送指令关');
        }
      })
    }
  },
})
