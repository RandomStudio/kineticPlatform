## What's in the box

- [Typescript](https://www.typescriptlang.org/): tame the javascript
- [Prettier](https://prettier.io/): stop fiddling with style
- [rc](https://www.npmjs.com/package/rc) + [parse-strings-in-object](https://github.com/anselanza/parse-strings-in-object): make config predictable
- [log4js](https://www.npmjs.com/package/log4js): timestamped logs with log levels

## Get started

Make a new template from the repo:

```
npx degit https://github.com/anselanza/nodejs-template#main my-new-project
```

Install the stuff

```
npm i
```

This also runs `postinstall` which will do the following:

- Compile everything `npm run build`
- Initialise a new git repo `git init`

Tidy up (might automate this soon):

- Please modify the package name, etc. in `package.json`!
- In `src/defaults.ts`, you should set `appName` to something more relevant, so that logs are labelled properly
- Modify `README.md` for your project
