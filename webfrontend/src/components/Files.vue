<template>
  <v-card>
    <v-data-table
      :headers="headers"
      :items="files"
      :items-per-page="5"
      class="elevation-1"
    >
      <template v-slot:item.actions="{ item }">
        <v-icon small class="mr-2" @click="play(item)"> mdi-play </v-icon>
      </template>
    </v-data-table>
  </v-card>
</template>

<script>
// import _ from 'lodash'
import axios from "axios";

export default {
  name: "Files",

  components: {},

  data() {
    return {
      files: [],
      headers: [
        {
          text: "Name",
          value: "name",
        },
        {
          text: "Size",
          value: "size",
        },
        { text: "Actions", value: "actions", sortable: false },
      ],
    };
  },

  beforeCreate: function () {},

  watch: {},

  mounted: async function () {
    const response = await axios({
      method: "get",
      url: "http://192.168.178.129/list?dir=/gifs",
      responseType: "application/json",
    });

    if (response.data) {
      this.files = response.data;
    }
  },

  methods: {
    play(item) {
      this.$socket.sendObj({
        action: "PLAY_GIF",
        name: item.name,
      });
    },
  },
};
</script>
<style>
</style>
