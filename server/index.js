const { spawn } = require('child_process')
const express = require('express')
const app = express()
const server = app.listen(3000)
const io = require('socket.io').listen(server)
const cors = require('cors')

app.use(cors({origin: true, credentials: true}))

io.on('connection', client => {
  console.log('connect')
  client.on('event', data => { /* … */ })
  client.on('disconnect', () => { /* … */ })
})

const sensor_read = spawn("./../sensor_read")
const sensor_vals = {
  'GYRO_X': 0,
  'GYRO_Y': 0,
  'GYRO_Z': 0,
  'ACC_X': 0,
  'ACC_Y': 0,
  'ACC_Z': 0
}
const readings = ['GYRO_X\r\n', 'GYRO_Y\r\n', 'GYRO_Z\r\n', 'ACC_X\r\n', 'ACC_Y\r\n', 'ACC_Z\r\n']


app.use('/', express.static('../client/dist/index.html'))

let chunk = ""
let removal = ""
sensor_read.stdout.on('data', data => {
  chunk += data.toString()
  let elms = chunk.match(/[^\n]+(?:\r?\n|$)/g)
  for (let el of elms) {
    if (el.indexOf('\n') >= 0) {
      removal += el
      let split = el.split(':')
      sensor_vals[split[0]] = parseInt(split[1].replace('\n', ''))
    }
  }
  chunk.replace(removal, '')
  removal = ''
})

function readValues() {
  io.emit('broadcast', JSON.stringify(sensor_vals))
  setTimeout(readValues, 500)
}
readValues()
