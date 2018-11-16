// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace gvs {
namespace util {

class GrpcClient;

template <typename Service>
class DualGrpcClient;

class GrpcServer;

} // namespace util

namespace host {

class SceneService;

} // namespace host

namespace vis {
namespace detail {

class Theme;

} // namespace detail

class VisClient;

} // namespace vis
} // namespace gvs
