<template>
  <v-row>
    <v-col cols="12">
      <v-btn
        class="float-right"
        color="primary"
        type="submit"
        :disabled="loading || error || !formValid"
        :loading="!!writing"
        depressed
      >
        {{ title }}
      </v-btn>
    </v-col>
  </v-row>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop } from "vue-property-decorator";

@Component
export default class ConfigurationFormButton extends Vue {
  @Prop({
    required: true
  })
  private error!: boolean;

  @Prop({
    required: true
  })
  private loading!: boolean;

  @Prop({
    required: true
  })
  private formValid!: boolean;

  @Prop({
    required: true
  })
  private writing!: boolean;

  @Prop({
    required: true
  })
  private title!: boolean;
}
</script>
