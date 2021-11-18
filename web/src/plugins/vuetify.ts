import Vue from "vue";
import Vuetify from "vuetify/lib/framework";
import colors from "vuetify/lib/util/colors";

Vue.use(Vuetify);

export default new Vuetify({
  theme: {
    dark: true,
    themes: {
      light: {
        primary: colors.green.base,
        secondary: colors.green.darken3,
        accent: colors.red.base,
        background: colors.grey.lighten4
      },
      dark: {
        primary: colors.cyan.base,
        secondary: colors.cyan.darken3,
        accent: colors.red.base,
        background: colors.grey.darken4
      }
    },
    options: {
      customProperties: true
    }
  }
});
