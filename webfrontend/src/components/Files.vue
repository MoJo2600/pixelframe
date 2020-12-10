<template>
  <v-card>
      <v-data-table
    :headers="headers"
    :items="files"
    :items-per-page="5"
    class="elevation-1"
  ></v-data-table>
  </v-card>
</template>

<script>
// import _ from 'lodash'
import axios from 'axios'

export default {
  name: "Files",

  components: {
  },

  data: () => ({
    files: null,
    headers: [
      {
        text: 'Name',
        value: 'name'
      },
      {
        text: 'Size',
        value: 'size'
      }
    ]
  }),

  beforeCreate: function () {
  },

  watch: {
  },

  mounted: async function () {
    this.files = await axios.get('192.168.178.129/list?dir=/gifs');
  },

  methods: {
  }
};
</script>
<style>
</style>
