<template>
  <span v-if="visible" class="ml-4 text-overline primary--text">
    {{ text }}
    <v-progress-circular
      class="ml-2"
      :width="2"
      :size="10"
      indeterminate
    ></v-progress-circular>
  </span>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop } from "vue-property-decorator";

@Component
export default class SpinnerText extends Vue {
  @Prop({
    required: true
  })
  private visible!: boolean;

  @Prop({
    required: true
  })
  private text!: boolean;
}
</script>
