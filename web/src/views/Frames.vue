<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Frames
          <spinner-text text="loading" v-if="loading" />
          <spinner-text text="sending" v-if="writing" />
        </h4>
      </v-col>
    </v-row>

    <v-row>
      <v-col :cols="cardSize" v-for="frame in frames" :key="frame.name">
        <v-card elevation="0" outlined @click="showFrame(frame)">
          <v-img
            :src="`https://picsum.photos/seed/${frame.apiPath}/600/300`"
            height="200px"
          ></v-img>

          <v-card-title>
            {{ frame.name }}
          </v-card-title>

          <v-card-subtitle>
            {{ frame.description }}
          </v-card-subtitle>

        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script lang="ts">
import Component from "vue-class-component";
import { Mixins } from "vue-property-decorator";
import SpinnerText from "@/components/SpinnerText.vue";
import DataLoaderError from "@/components/DataLoaderError.vue";
import ConfigurationSection from "@/components/ConfigurationSection.vue";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import { DataHandlerMixin, WriteAction } from "@/mixins";
import { Service, FramesService } from "@/services";

interface Frame {
  name: string;
  description: string;
  apiPath: string;
}

@Component({
  components: {
    SpinnerText,
    DataLoaderError,
    ConfigurationSection,
    ConfigurationInputWrapper
  }
})
export default class FramesView extends Mixins(DataHandlerMixin) {
  private readonly framesService = Service.get(FramesService);

  private readonly frames: Frame[] = [
    {
      name: "Pong Clock",
      description: "A fancy pong clock",
      apiPath: "clock"
    },
    {
      name: "Gif",
      description: "Plays random GIF animations.",
      apiPath: "gif"
    },
    {
      name: "Visuals",
      description: "Plays random visuals.",
      apiPath: "visuals"
    },
    {
      name: "Off",
      description: "Turn off the LEDs.",
      apiPath: "off"
    }
  ];

  private get cardSize(): number {
    return this.$vuetify.breakpoint.xs
      ? 12
      : this.$vuetify.breakpoint.sm
      ? 6
      : this.$vuetify.breakpoint.md
      ? 3
      : this.$vuetify.breakpoint.lg
      ? 3
      : 3;
  }

  private async showFrame(frame: Frame): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        await this.framesService.showFrame(frame.apiPath);
      },
      WriteAction.Command,
      `show frame ${frame.name}`
    );
  }

  private async created(): Promise<void> {
    await this.wrapDataRead(async () => {
      // nothing to load, required for setting loading to false
    });
  }
}
</script>
