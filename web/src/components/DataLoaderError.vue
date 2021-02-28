<template>
  <v-expand-transition>
    <v-row>
      <v-col cols="6" offset="3">
        <v-card elevation="0" style="background: none" outlined>
          <v-card-text>
            <h6 class="text-h6 text-center">
              {{ text }}
            </h6>
            <v-flex text-center align-center>
              <v-btn color="accent" plain @click="retry">
                <v-icon left color="accent"> mdi-refresh </v-icon>Retry
              </v-btn>
            </v-flex>
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
  </v-expand-transition>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop } from "vue-property-decorator";
import RendererModule from "@/store/modules/renderer";

@Component
export default class DataLoaderError extends Vue {
  @Prop({
    required: true
  })
  private text!: string;

  private retry(): void {
    RendererModule.forceReRender();
  }
}
</script>
