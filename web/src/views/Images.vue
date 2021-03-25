<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Images
          <spinner-text text="loading" v-if="loading" />
        </h4>
      </v-col>
    </v-row>

    <data-loader-error v-if="error" text="Failed to load images" />

    <configuration-section title="Upload image" :error="error">
      <v-form v-model="formValid" @submit.prevent="uploadImage">
        <v-row>
          <v-col cols="12">
            <v-file-input
              label="File"
              v-model="file"
              outlined
              :rules="[required]"
              accept="image/*"
              hide-details
            ></v-file-input>
          </v-col>
        </v-row>

        <configuration-form-button
          :error="error"
          :loading="loading"
          :formValid="formValid"
          :writing="writing"
          title="Upload"
        />
      </v-form>
    </configuration-section>

    <configuration-section :error="error">
      <v-row>
        <v-col :cols="imageSize" v-for="image in images" :key="image.name">
          <v-hover>
            <template v-slot:default="{ hover }">
              <v-container class="ma-0 pa-0" style="position: relative">
                <v-img
                  class="pixelated"
                  :class="{ 'image-hover': hover }"
                  :src="imageHost + '/' + image.name"
                ></v-img>

                <v-fade-transition>
                  <v-overlay v-if="hover" absolute>
                    <v-btn
                      icon
                      large
                      color="white"
                      @click="playImage(image.name)"
                    >
                      <v-icon>mdi-play</v-icon>
                    </v-btn>

                    <v-btn
                      icon
                      large
                      color="red"
                      @click="deleteImage(image.name)"
                    >
                      <v-icon>mdi-delete</v-icon>
                    </v-btn>
                  </v-overlay>
                </v-fade-transition>
              </v-container>
            </template>
          </v-hover>
        </v-col>
      </v-row>
    </configuration-section>
  </v-container>
</template>

<script lang="ts">
import Component from "vue-class-component";
import { Mixins } from "vue-property-decorator";
import SpinnerText from "@/components/SpinnerText.vue";
import DataLoaderError from "@/components/DataLoaderError.vue";
import ConfigurationSection from "@/components/ConfigurationSection.vue";
import ConfigurationFormButton from "@/components/ConfigurationFormButton.vue";
import { DataHandlerMixin, WriteAction } from "@/mixins";
import { Image } from "@/models/images";
import { Service, ImagesService } from "@/services";
import { required } from "@/validation";

@Component({
  components: {
    SpinnerText,
    DataLoaderError,
    ConfigurationSection,
    ConfigurationFormButton
  }
})
export default class ImagesView extends Mixins(DataHandlerMixin) {
  required = required;

  private readonly imgService = Service.get(ImagesService);
  private imageHost = `${process.env.VUE_APP_API_BASE_URL ||
    window.location.origin}/api/images`;
  private images: Image[] = [];
  private file: string | null = null;
  private formValid = false;

  private get imageSize(): number {
    return this.$vuetify.breakpoint.xs
      ? 6
      : this.$vuetify.breakpoint.sm
      ? 4
      : this.$vuetify.breakpoint.md
      ? 4
      : this.$vuetify.breakpoint.lg
      ? 2
      : 1;
  }

  private async playImage(name: string): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        await this.imgService.playImage(name);
      },
      WriteAction.Command,
      `play image ${name}`
    );
  }

  private async deleteImage(name: string): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        await this.imgService.deleteImage(name);
      },
      WriteAction.Delete,
      `image ${name}`
    );
  }

  private async uploadImage(): Promise<void> {
    console.log(this.file);
  }

  private async created(): Promise<void> {
    await this.wrapDataRead(async () => {
      this.images = await this.imgService.listImages();
    });
  }
}
</script>

<style lang="scss" scoped>
.pixelated {
  background: #000000;
}

::v-deep .v-image__image {
  image-rendering: pixelated;
}

.image-hover {
  transition: opacity 300ms, filter 300ms;
  filter: grayscale(100%);
  opacity: 0.4;
}
</style>
