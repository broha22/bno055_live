const { spawn } = require('child_process')
const Readable = require('stream').Readable

const server = require('http').createServer()
const io = require('socket.io')(server)
io.on('connection', client => {
  client.on('event', data => { /* … */ })
  client.on('disconnect', () => { /* … */ })
})

const sensor_read = spawn("./../sensor_read")
const sensor_vals = {
  'GYR_X': 0,
  'GYR_Y': 0,
  'GYR_Z': 0,
  'ACC_X': 0,
  'ACC_Y': 0,
  'ACC_Z': 0
}
const readings = ['GYR_X\n', 'GYR_Y\n', 'GYR_Z\n', 'ACC_X\n', 'ACC_Y\n', 'ACC_Z\n', '\n']

sensor_read.stdout.on('data', data => {
  // const split_data = data.split(":")
  // sensor_vals[split_data[0]] = split_data[1]
  console.log(data.toString())
})

for (let reading of readings) {
  const buffer = new Readable()
  buffer.push(reading)
  buffer.push(null)
  buffer.pipe(sensor_read.stdin)
} 

io.emit('broadcast', /* */)
server.listen(3000)

function exitHandler(options, exitCode) {
    if (options.cleanup) console.log('clean')
    if (exitCode || exitCode === 0) console.log(exitCode)

    if (options.exit) {
      const buffer = new Readable()
      buffer.push("q\n")
      buffer.push(null)
      buffer.pipe(sensor_read.stdin)
      sensor_read.on('exit', function() {
        process.exit()
      })
    }
}

//do something when app is closing
process.on('exit', exitHandler.bind(null,{cleanup:true}))

//catches ctrl+c event
process.on('SIGINT', exitHandler.bind(null, {exit:true}))

// catches "kill pid" (for example: nodemon restart)
process.on('SIGUSR1', exitHandler.bind(null, {exit:true}))
process.on('SIGUSR2', exitHandler.bind(null, {exit:true}))

//catches uncaught exceptions
process.on('uncaughtException', exitHandler.bind(null, {exit:true}))
