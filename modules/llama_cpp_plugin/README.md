### Build instructions

This plugin should be built in the same fashion as the rest of the modules:

1. Check out the OpenVINO repository proper (https://github.com/openvinotoolkit/openvino)
2. Configure the CMake build of the OpenVINO repository, making sure to point the corresponding CMake option to the location of the `openvino_contrib` repository. The command below, executed in the `openvino` repo root, will configure the build so that the modules other `llama_cpp_plugin` module will not be built to save build time - adjust the `-DBUILD_*` options if you need the other modules as well.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DOPENVINO_EXTRA_MODULES=<PATH_TO_YOUR_CHECKED_OUT_OPENVINO_CONTRIB>/modules/llama_cpp_plugin -DENABLE_PLUGINS_XML=ON -DENABLE_PLUGIN_REGISTRATION=ON .
```

3. Build the plugin either as part of the complete openvino build by executing:

```bash
cmake --build build --parallel
```

or separately by specifying only the `llama_cpp_plugin` target:

```bash
cmake --build build --parallel -- llama_cpp_plugin
```

4. Now you can utilize the built `libllama_cpp_plugin.so` as a regular OV plugin with the device name `"LLAMA_CPP"` to directly load GGUF files and infer them using OV API with llama.cpp execution under the hood. Make sure that the plugin is discoverable by the OV runtime (e.g. by putting the built `libllama_cpp_plugin.so`, `libllama.so` and the autogenerated `plugins.xml` from the built location to your OV binaries location, or by setting `LD_LIBRARY_PATH` appropriately).

#### Example of LLM inference code

```C++

ov::Core core;
auto model = core.compile_model("model.gguf", "LLAMA_CPP")
auto input_ids = ov::Tensor(ov::element::Type_t::i64, {1, 128});
auto position_ids = ov::Tensor(ov::element::Type_t::i64, {1, 128});
std::iota(position_ids.data<int64_t>(), position_ids.data<int64_t>() + position_ids.get_size(), 0);

auto infer_request == model.create_infer_request();
infer_request.set_tensor("input_ids", input_ids);
infer_request.set_tensor("position_ids", position_ids);
infer_request.infer();

size_t vocab_size = lm.get_tensor("logits").get_shape().back();
float* logits = lm.get_tensor("logits").data<float>() + (input_ids_tensor.get_size() - 1) * vocab_size;
int64_t out_token = std::max_element(logits, logits + vocab_size) - logits;
```

The models obtained by the `.compile_model` call with the `LLAMA_CPP` plugin expose two inputs (`input_ids` and `position_ids`) and a single output (`logits`) with equivalent meaning to the corresponding arguments in the LLM model representations in the huggingface `transformers` repository. The `attention_mask` and `beam_idx` inputs may be set as well, but will have no effect on the execution.

Only batch size of 1 is currently supported.



