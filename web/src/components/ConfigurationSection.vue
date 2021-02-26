<template>
  <v-row>
    <v-col cols="12">
      <v-card :disabled="error" elevation="0" outlined>
        <v-card-text>
          <v-row v-if="title">
            <v-col cols="12">
              <p class="text-h6">{{ title }}</p>
            </v-col>
          </v-row>

          <slot></slot>
        </v-card-text>
      </v-card>
    </v-col>
  </v-row>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop } from "vue-property-decorator";

@Component
export default class ConfigurationSection extends Vue {
  @Prop({
    required: true
  })
  private error!: boolean;

  @Prop({
    required: false
  })
  private title?: string;
}
</script>
