import zh_CN from "./i18n/zh_CN.js"
import en from "./i18n/en.js"

const messages = {
	"en": en,
	"zh_CN": zh_CN,
};

const i18n = new VueI18n({
	messages, // set locale messages
});

export default i18n;