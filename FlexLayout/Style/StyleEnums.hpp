// enumの追加方法
// 1. ディレクトリ Enum/ に新しいヘッダファイルを追加
// 2. Enum/Common.hppのStyleEnumTypeIDにenum名を追加
// 3. 新しいファイル内にenum classとdetail::style_enum_traitsを実装
// 4. このファイルに #include "Enum/[新しいenum名].hpp" を追加

#include "Enum/AlignContent.hpp"
// #include "Enum/[...].hpp"
