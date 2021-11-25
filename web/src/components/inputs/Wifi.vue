<template>
  <v-container>
    <configuration-input-wrapper
      title="Network"
      skeletonType="input"
      :error="error"
      :loading="loading"
    >
      <!-- old variant with autocomplete
      <v-autocomplete
        v-if="!loading && !error"
        v-model="ssid"
        :items="wifiSsidsSorted"
        outlined
        hide-details
        :rules="[required]"
      ></v-autocomplete> -->

      <v-radio-group v-model="wifi" :rules="[required]">
        <v-card
          v-for="w of sortedWifis"
          :key="w.ssid"
          class="px-4"
          @click="wifi = w"
        >
          <v-card-text>
            <v-row class="wifi py-3">
              <v-col cols="auto">
                <v-icon
                  class="wifiIcon pr-2"
                  :class="{
                    selected: wifi && wifi.ssid === w.ssid
                  }"
                  >mdi-wifi</v-icon
                >
              </v-col>

              <v-col>
                <v-row>
                  <p>{{ w.ssid }}</p>
                </v-row>
                <v-row class="mt-0"
                  ><small class="text--disabled"
                    >Signal strength: {{ w.signalStrength }}</small
                  ></v-row
                >
              </v-col>

              <v-radio :value="w.ssid" class="d-none"></v-radio>
            </v-row>
          </v-card-text>
        </v-card>
      </v-radio-group>

      <v-btn color="primary" @click="onRefreshClick()">
        <v-icon left>
          mdi-refresh
        </v-icon>
        Refresh
      </v-btn>
    </configuration-input-wrapper>
  </v-container>
</template>

<script lang="ts">
import { orderBy } from "lodash";
import Vue from "vue";
import Component from "vue-class-component";
import { Emit, Prop, VModel } from "vue-property-decorator";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import { required } from "@/validation";
import { Wifi } from "@/models/environment";

@Component({
  components: {
    ConfigurationInputWrapper
  }
})
export default class WifiInput extends Vue {
  required = required;

  @VModel()
  public wifi?: Wifi;

  @Prop()
  public availableWifis!: Wifi[];

  @Prop()
  public loading!: boolean;

  @Prop()
  public error!: boolean;

  @Emit()
  public onRefreshClick() {
    return true;
  }

  public get sortedWifis() {
    return orderBy(this.availableWifis, ["signalStrength"], "desc");
  }
}
</script>

<style>
.wifiIcon.selected {
  opacity: 1;
  color: var(--v-primary-base);
}

.wifiIcon {
  opacity: 0.4;
}
</style>
