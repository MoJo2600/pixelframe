<template>
  <v-container>
    <v-row>
      <v-col class="pb-0">
        <p class="text-body-2 ma-0 pl-2">{{ title }}</p>
      </v-col>
    </v-row>
    <v-row>
      <v-col>
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
  </v-container>
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
  public loading!: boolean;

  @Prop({
    required: true
  })
  public error!: boolean;

  @Prop({
    required: true
  })
  public title!: string;

  @Prop({
    required: true
  })
  public skeletonType!: SkeletonType;
}
</script>

<style lang="scss">
.skeleton-child-full-width {
  & > div {
    width: 100%;
  }
}
</style>
