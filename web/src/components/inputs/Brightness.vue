<template>
  <v-container>
    <configuration-input-wrapper
      title="Brightness"
      skeletonType="slider"
      :error="error"
      :loading="loading"
    >
      <v-slider
        v-if="!loading && !error"
        min="0"
        max="255"
        v-model="brightness"
        hide-details
      ></v-slider>
    </configuration-input-wrapper>

    <v-container>
      <v-row class="pl-2">
        <v-col class="py-0">
          <v-skeleton-loader
            v-if="loading || error"
            :boilerplate="error"
            class="mt-1 mb-3"
            max-width="50%"
            type="text"
          ></v-skeleton-loader>
          <p v-else class="text-caption">Absolute value: {{ brightness }}</p>
        </v-col>
      </v-row>

      <v-row class="pl-2">
        <v-col class="py-0">
          <v-skeleton-loader
            v-if="loading || error"
            :boilerplate="error"
            class="mt-1 mb-3"
            max-width="50%"
            type="text"
          ></v-skeleton-loader>
          <p v-else class="text-caption">
            Percent value: {{ ((brightness / 255) * 100).toFixed(0) }}%
          </p>
        </v-col>
      </v-row>
    </v-container>
  </v-container>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop, VModel } from "vue-property-decorator";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import { required } from "@/validation";

@Component({
  components: {
    ConfigurationInputWrapper
  }
})
export default class BrigthnessInput extends Vue {
  required = required;

  @VModel({ type: Number })
  public brightness!: number;

  @Prop()
  public loading!: boolean;

  @Prop()
  public error!: boolean;
}
</script>
