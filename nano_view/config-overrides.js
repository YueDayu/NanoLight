const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const CssMinimizerPlugin = require('css-minimizer-webpack-plugin');
const TerserWebpackPlugin = require('terser-webpack-plugin');

module.exports = function override(config, env) {
  const cssFilenameTemplate = '[name].[contenthash:8].css';
  const jsFilenameTemplate = '[name].[contenthash:8].js';
  const mediaFilenameTemplate = '[name].[contenthash:8].[ext]';

  const plugins = ((config && config.plugins) || []).map((a) => {
    if (a instanceof MiniCssExtractPlugin) {
      return new MiniCssExtractPlugin({
        ...a.options,
        filename: cssFilenameTemplate,
        chunkFilename: cssFilenameTemplate,
      });
    }

    return a;
  });

  const rules = ((config && config.module && config.module.rules) || []).map(
    (a) => {
      if (a && Array.isArray(a.oneOf)) {
        return {
          oneOf: a.oneOf.map((b) => {
            if (
              b &&
              typeof b.loader === 'string' &&
              (b.loader.includes('url-loader') ||
                b.loader.includes('file-loader'))
            ) {
              return {
                ...b,
                options: {
                  ...(b.options || {}),
                  name: mediaFilenameTemplate,
                },
              };
            }

            return b;
          }),
        };
      }

      return a;
    },
  );

  return {
    ...config,
    output: {
      ...config.output,
      filename: jsFilenameTemplate,
      chunkFilename: jsFilenameTemplate,
    },
    plugins,
    module: {
      ...config.module,
      rules,
    },
    optimization: {
      minimizer: [
        new TerserWebpackPlugin({
          parallel: true,
          extractComments: false,
          terserOptions: {
            compress: {drop_console: true, drop_debugger: true},
          },
        }),
        new CssMinimizerPlugin(),
      ],
    },
  };
}
