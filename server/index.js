const { spawn } = require('child_process')
const express = require('express')
const app = express()
const server = app.listen(3000)
const io = require('socket.io').listen(server)
const cors = require('cors')
const mysql = require('mysql')
const bodyParser = require('body-parser')
const path = require('path')

app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.json())

const db = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: 'raspberry',
  port: 3306,
  database: 'sensor_data'
})

function queryDB(sql, args) {
  return new Promise((resolve, reject) => {
    db.query(sql, args, (err, rows) => {
      if (err) { return reject(err) }
      resolve(rows)
    })
  })
}

app.use(cors({origin: true, credentials: true}))

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


app.get('/download', async (req, res) => {
  let acc_x = await queryDB('SELECT time as x, sensor_val as y FROM data_acc_x WHERE run_id = ? ORDER BY time ASC', req.query.id)
  let acc_y = await queryDB('SELECT time as x, sensor_val as y FROM data_acc_y WHERE run_id = ? ORDER BY time ASC', req.query.id)
  let acc_z = await queryDB('SELECT time as x, sensor_val as y FROM data_acc_z WHERE run_id = ? ORDER BY time ASC', req.query.id)
  let gyro_x = await queryDB('SELECT time as x, sensor_val as y FROM data_gyro_x WHERE run_id = ? ORDER BY time ASC', req.query.id)
  let gyro_y = await queryDB('SELECT time as x, sensor_val as y FROM data_gyro_y WHERE run_id = ? ORDER BY time ASC', req.query.id)
  let gyro_z = await queryDB('SELECT time as x, sensor_val as y FROM data_gyro_z WHERE run_id = ? ORDER BY time ASC', req.query.id)
  
  res.send(JSON.stringify({
    'GYRO X': gyro_x,
    'GYRO Y': gyro_y,
    'GYRO Z': gyro_z,
    'ACC X': acc_x,
    'ACC Y': acc_y,
    'ACC Z': acc_z,
  }))
})


async function uploadTableData(tableName, run_id, entries) {
  for (let entry of entries) {
    queryDB('INSERT INTO ' + tableName + ' (run_id, time, sensor_val) VALUES (?, ?, ?)', [
      run_id,
      entry.x,
      entry.y
    ])
  }
}

app.get('/runs', async (req, res) => {
  let query = (await queryDB('SELECT run_id, time FROM data_acc_x GROUP BY run_id ORDER BY run_id DESC'))
  res.send(JSON.stringify(query))
})

app.post('/upload', async (req, res) => {
  let query = (await queryDB('SELECT run_id FROM data_acc_x ORDER BY run_id DESC LIMIT 1'))
  let run_id = 1
  if (query.length > 0) {
    run_id = parseInt(query[0].run_id) + 1
  } 
  await uploadTableData('data_acc_x', run_id, req.body['ACC X'])
  await uploadTableData('data_acc_y', run_id, req.body['ACC Y'])
  await uploadTableData('data_acc_z', run_id, req.body['ACC Z'])
  await uploadTableData('data_gyro_x', run_id, req.body['GYRO X'])
  await uploadTableData('data_gyro_y', run_id, req.body['GYRO Y'])
  await uploadTableData('data_gyro_z', run_id, req.body['GYRO Z'])
  res.status(200).send()
})
app.use(express.static('../client/dist'))
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '../client/dist/index.html'))
})

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
  // io.emit('broadcast', JSON.stringify(sensor_vals))
  setTimeout(readValues, 1000)
}
readValues()

io.on('connection', client => {
  function readValues() {
    client.emit('data', JSON.stringify(sensor_vals))
    if (client.connected) setTimeout(readValues, 1000)
  }
  readValues()
  client.on('event', data => { /* … */ })
  client.on('disconnect', () => { /* … */ })
})