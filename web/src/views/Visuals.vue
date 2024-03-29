<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Visuals
          <spinner-text text="loading" v-if="loading" />
          <spinner-text text="sending" v-if="writing" />
        </h4>
      </v-col>
    </v-row>

    <v-row>
      <v-col :cols="cardSize" v-for="visual in visuals" :key="visual.name">
        <v-card elevation="0" outlined @click="showVisual(visual)">
          <v-img
            :src="`https://picsum.photos/seed/${visual.name}/600/300`"
            height="200px"
          ></v-img>

          <v-card-title>
            {{ visual.title }}
          </v-card-title>

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
import { Service, VisualsService } from "@/services";

interface Visual {
  title: string;
  name: string;
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
  private readonly visualsService = Service.get(VisualsService);

  // TODO: Load this from the backend
  private readonly visuals: Visual[] = [
    {
      title: "Bpm",
      name: "bpm"
    },
    {
      title: "Twinkle",
      name: "twinkle"
    },
    {
      title: "Pacifica",
      name: "pacifica"
    },
    {
      title: "Rainbow Beat",
      name: "rainbowbeat"
    },
    {
      title: "Confetti",
      name: "confetti"
    },
    {
      title: "Noise",
      name: "noise"
    },
    {
      title: "Random",
      name: "random"
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

  private async showVisual(visual: Visual): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        await this.visualsService.showVisual(visual.name);
      },
      WriteAction.Command,
      `show visual ${visual.name}`
    );
  }

  private async created(): Promise<void> {
    await this.wrapDataRead(async () => {
      // nothing to load, required for setting loading to false
    });
  }
}
</script>
