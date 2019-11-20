import { Line, mixins } from 'vue-chartjs'
const { reactiveProp } = mixins

export default {
  extends: Line,
  mixins: [reactiveProp],
  props: ['options'],
  mounted () {
    // this.chartData is created in the mixin.
    // If you want to pass options please create a local options object
    this.renderChart(this.chartData, {
      maintainAspectRatio: false,
      scales: {
        xAxes: [{
          type: 'time',
          bounds: 'data',
          gridLines: {
            display: false
          }
        }]
      }
    })
  },
  methods: {
    update: function () {
      this.$data._chart.update()
    }
  }
}