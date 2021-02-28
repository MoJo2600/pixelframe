<template>
  <router-view></router-view>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";

@Component({})
export default class NestedViewWrapper extends Vue {}
</script>
