<template>
  <v-row>
    <v-col :cols="$vuetify.breakpoint.xs ? 12 : 4">
      <p class="text-body-1">{{ title }}</p>
    </v-col>
    <v-col :cols="$vuetify.breakpoint.xs ? 12 : 8">
      <v-skeleton-loader
        v-if="skeletonType === SkeletonType.Input && (loading || error)"
        :boilerplate="error"
        class="skeleton-child-full-width"
        type="button"
      ></v-skeleton-loader>

      <v-skeleton-loader
        v-if="skeletonType === SkeletonType.Slider && (loading || error)"
        :boilerplate="error"
        class="mx-auto mt-2"
        type="text"
      ></v-skeleton-loader>

      <slot></slot>
    </v-col>
  </v-row>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop } from "vue-property-decorator";

export enum SkeletonType {
  Input = "input",
  Slider = "slider"
}

@Component
export default class ConfigurationInputWrapper extends Vue {
  SkeletonType = SkeletonType;

  @Prop({
    required: true
  })
  private loading!: boolean;

  @Prop({
    required: true
  })
  private error!: boolean;

  @Prop({
    required: true
  })
  private title!: string;

  @Prop({
    required: true
  })
  private skeletonType!: SkeletonType;
}
</script>
