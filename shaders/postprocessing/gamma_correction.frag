vec4 gamma_correction(vec4 currentFragment, sampler2D screen) {
    return vec4(vec3(1.0) - currentFragment.rgb, 1.0);
}
