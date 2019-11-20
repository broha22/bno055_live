<template>
  <div id="app">
    <div class='button_bar'>
      <div class='web_title'>
        BNO055 Sensor
      </div>
      <div class='buttons'>
        <div class='button save_data'>
          Save Data
        </div>
        <div class='button open_data'>
          Open Data
        </div>
        <div class='button clear_data'>
          Clear Data
        </div>
      </div>
    </div>
    <div class='chart_container'>
      <line-chart :chart-data="datacollection" ref='chart' style='position: absolute; width: 100%; height: 100%;'></line-chart>
    </div>
    <div class="sensors">
      <div class='live_sensor'>
        <div class='title'>Accelerometer</div>
        <div class='axis'>x: {{ ACC_X }}</div>
        <div class='axis'>y: {{ ACC_Y }}</div>
        <div class='axis'>z: {{ ACC_Z }}</div>
      </div>
      <div class='live_sensor'>
        <div class='title'>Gyroscope</div>
        <div class='axis'>x: {{ GYRO_X }}</div>
        <div class='axis'>y: {{ GYRO_Y }}</div>
        <div class='axis'>z: {{ GYRO_Z }}</div>
      </div>
    </div>
  </div>
</template>

<script>
import io from 'socket.io-client'
import LineChart from '@/components/line-chart.js'
export default {
  name: 'app',
  components: {
    LineChart
  },
  data () {
    return {
      ACC_X: 0,
      ACC_Y: 0,
      ACC_Z: 0,
      GYRO_X: 0,
      GYRO_Y: 0,
      GYRO_Z: 0,
      datacollection: {
        labels: [],
        datasets: [{
            label: 'GYRO X',
            backgroundColor: '#d73027',
            borderColor: '#d73027',
            fill: false,
            data: []
          },{
            label: 'GYRO Y',
            backgroundColor: '#fc8d59',
            borderColor: '#fc8d59',
            fill: false,
            data: []
          },{
            label: 'GYRO Z',
            backgroundColor: '#fee090',
            borderColor: '#fee090',
            fill: false,
            data: []
          },{
            label: 'ACC X',
            backgroundColor: '#e0f3f8',
            borderColor: '#e0f3f8',
            fill: false,
            data: []
          },{
            label: 'ACC Y',
            backgroundColor: '#91bfdb',
            borderColor: '#91bfdb',
            fill: false,
            data: []
          },{
            label: 'ACC Z',
            backgroundColor: '#4575b4',
            borderColor: '#4575b4',
            fill: false,
            data: []
          }]
      }
    }
  },
  created() {
    let socket = io('http://192.168.1.132:3000')
    socket.on('broadcast', data => {
      let parsed = JSON.parse(data)
      this.ACC_X = parsed.ACC_X
      this.ACC_Y = parsed.ACC_Y
      this.ACC_Z = parsed.ACC_Z
      this.GYRO_X = parsed.GYRO_X
      this.GYRO_Y = parsed.GYRO_Y
      this.GYRO_Z = parsed.GYRO_Z
      for (let set of this.datacollection.datasets) {
        set.data.push({ x: (new Date()), y: this[set.label.replace(' ', '_')] })
        this.$refs.chart.update()
      }
    })
  }
}
</script>

<style>
body {
  padding: 0;
  margin: 0;
  background-color: #303030;
}

#app {
  width: 100%;
}

.sensors {
  background-color: #1a1a1a;
  height: 150px;
}
.live_sensor {
  display: inline-block;
  width: calc(50% - 24px);
  text-align: center;
  color: #fff;
  border: solid 2px #aaaaaa;
  border-radius: 10px;
  margin: 10px;
  font-family: Arial, Helvetica, sans-serif;
}

.title {
  padding: 8px;
  font-size: 20px;
  font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;
}
.axis {
  padding: 2px;
}
.axis:nth-child(1) {
  padding: 8px;
}
.chart_container {
  position: relative;
  top: 0;
  left: 0;
  height: calc(100vh - 250px);
  width: calc(100vw - 25px);
  margin: 10px;
}

.button_bar {
  width: 100%;
  height: 100px;
  background-color: #1a1a1a;
  padding: 0;
  margin: 0;
}
.web_title {
  display: inline-block;
  width: 50%;
  padding-left: 20px;
  padding-top: 20px;
  font-size: 48px;
  color: #fff;
  font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
}
.buttons {
  position: absolute;
  top: 0;
  right: 0;
  padding-top: 8px;
  padding-right: 3px;
  text-align: right;
  display: inline-block;
  width: 40%;
}

.button {
  position: relative;
  right: 10px;
  display: inline-block;
  padding: 14px;
  color: #fff;
  border: solid 2px #aaaaaa;
  border-radius: 10px;
  font-size: 20px;
  margin: 15px;
  font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;
}
.open_data {
  background-color: #4a4afa;
}
.save_data {
  background-color: #5aaa4a;
}
.clear_data {
  background-color: #fa4a4a;
}
</style>
