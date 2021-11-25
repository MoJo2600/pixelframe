<template>
  <v-expand-transition>
    <v-container v-show="show">
      <configuration-input-wrapper
        title="Password"
        skeletonType="input"
        :error="error"
        :loading="loading"
      >
        <v-text-field
          v-if="!loading && !error"
          v-model="password"
          outlined
          :append-icon="showWifiPassword ? 'mdi-eye' : 'mdi-eye-off'"
          @click:append="showWifiPassword = !showWifiPassword"
          :type="showWifiPassword ? 'text' : 'password'"
          :rules="[required]"
          hide-details
        ></v-text-field>
      </configuration-input-wrapper>
    </v-container>
  </v-expand-transition>
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
export default class WifiPasswordInput extends Vue {
  required = required;
  public showWifiPassword = false;

  @VModel({ type: String })
  public password!: string;

  @Prop()
  public loading!: boolean;

  @Prop()
  public error!: boolean;

  @Prop()
  public show!: boolean;
}
</script>
