<template>
  <v-container>
    <configuration-input-wrapper
      title="Timezone"
      skeletonType="input"
      :error="error"
      :loading="loading"
    >
      <v-autocomplete
        v-if="!loading && !error"
        v-model="timezone"
        :items="timezoneItems"
        outlined
        hide-details
        :rules="[required]"
      ></v-autocomplete>
    </configuration-input-wrapper>
  </v-container>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop, VModel } from "vue-property-decorator";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import { required } from "@/validation";
import timezones from "@/assets/timezones.json";

@Component({
  components: {
    ConfigurationInputWrapper
  }
})
export default class TimezoneInput extends Vue {
  required = required;

  @VModel({ type: String })
  public timezone!: string;

  @Prop()
  public loading!: boolean;

  @Prop()
  public error!: boolean;

  public timezoneItems = timezones.map(t => {
    return {
      text: t,
      value: t
    };
  });
}
</script>
