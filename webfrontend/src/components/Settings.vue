<template>
  <v-container>
    <v-row
    justify="space-around">
      <v-col cols="12">
        <v-slider
          v-model="brightness"
          min="0"
          max="255"
          label="Brightness"
        ></v-slider>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
import _ from 'lodash'

export default {
  name: "Settings",

  components: {
  },

  data: () => ({
    brightness: 32
  }),

  beforeCreate: function () {
  },

  watch: {
    brightness: _.throttle(function(val) {
      if (!val) return;
      console.log("Send brightness");
      this.$socket.sendObj(
        { 
          brightness: this.brightness
        }
      )
    }, 50)
  },

  mounted: function() {
    this.$options.sockets.onmessage = message => {
      // const jsonData = JSON.parse(message.data);
      console.log(message.data);
    };
  },

  methods: {

    save: function() {
      this.$socket.sendObj(
        { 
        }
      )
      this.finishDialog = true;
    }
  }
};
</script>
<style>

</style>
