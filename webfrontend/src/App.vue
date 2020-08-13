<template>
  <v-app>
    <v-app-bar
      app
      color="primary"
      dark
    >
      <div class="d-flex align-center">
        <h2>PixelFrame</h2>
      </div>
      <v-spacer></v-spacer>
        
    </v-app-bar>

    <v-content>
      <settings></settings>
    </v-content>
  </v-app>
</template>

<script>
import Settings from './components/Settings';

export default {
  name: 'App',

  components: {
    Settings
  },

  data: () => ({
  }),

  watch: {
  },

  mounted: function() {
    this.$options.sockets.onmessage = message => {
      console.log(message);
      // const jsonData = JSON.parse(message.data);
      // TODO: do something when a message arrives
    }
  }
};
</script>
