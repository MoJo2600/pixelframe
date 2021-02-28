<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Basic configuration
          <spinner-text text="loading" v-if="loading" />
        </h4>
      </v-col>
    </v-row>

    <data-loader-error v-if="error" text="Failed to load basic configuration" />

    <configuration-section title="Lighting" :error="error">
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
          v-model="basicConfiguration.brightness"
          hide-details
          @end="updateBrightness"
        ></v-slider>
      </configuration-input-wrapper>

      <v-row style="margin-top: -24px">
        <v-col
          :cols="$vuetify.breakpoint.xs ? 12 : 8"
          :offset="$vuetify.breakpoint.xs ? 0 : 4"
        >
          <v-row>
            <v-col cols="6">
              <v-skeleton-loader
                v-if="loading || error"
                :boilerplate="error"
                class="mx-auto mt-1 mb-3"
                max-width="200"
                type="text"
              ></v-skeleton-loader>
              <p v-else class="text-caption text-center">
                Absolute value: {{ basicConfiguration.brightness }}
              </p>
            </v-col>
            <v-col cols="6">
              <v-skeleton-loader
                v-if="loading || error"
                :boilerplate="error"
                class="mx-auto mt-1 mb-3"
                max-width="200"
                type="text"
              ></v-skeleton-loader>
              <p v-else class="text-caption text-center">
                Percent value:
                {{ ((basicConfiguration.brightness / 255) * 100).toFixed(0) }}%
              </p>
            </v-col>
          </v-row>
        </v-col>
      </v-row>
    </configuration-section>

    <configuration-section title="Clock" :error="error">
      <configuration-input-wrapper
        title="Timezone"
        skeletonType="input"
        :error="error"
        :loading="loading"
      >
        <v-autocomplete
          v-if="!loading && !error"
          v-model="basicConfiguration.timezone"
          :items="timezoneItems"
          outlined
          dense
          hide-details
          @change="updateTimezone"
        ></v-autocomplete>
      </configuration-input-wrapper>
    </configuration-section>
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
import { BasicConfiguration } from "@/models/configuration";
import { Service, ConfigurationService } from "@/services";
import timezones from "@/assets/timezones.json";

@Component({
  components: {
    SpinnerText,
    DataLoaderError,
    ConfigurationSection,
    ConfigurationInputWrapper
  }
})
export default class BasicConfigurationView extends Mixins(DataHandlerMixin) {
  private readonly configService = Service.get(ConfigurationService);
  private basicConfiguration: BasicConfiguration | null = null;
  private timezoneItems = timezones.map(t => {
    return {
      text: t,
      value: t
    };
  });

  private async updateBrightness(): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        if (!this.basicConfiguration) {
          return;
        }

        await this.configService.updateBasicConfiguratin({
          brightness: this.basicConfiguration.brightness
        });
      },
      WriteAction.Update,
      "brightness"
    );
  }

  private async updateTimezone(): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        if (!this.basicConfiguration) {
          return;
        }

        await this.configService.updateBasicConfiguratin({
          timezone: this.basicConfiguration.timezone
        });
      },
      WriteAction.Update,
      "timezone"
    );
  }

  private async created(): Promise<void> {
    await this.wrapDataRead(async () => {
      this.basicConfiguration = await this.configService.getBasicConfiguration();
    });
  }
}
</script>
