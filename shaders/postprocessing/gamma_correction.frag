vec4 gamma_correction(vec4 currentFragment, sampler2D screen) {
    const float gamma = 2.2;
    return vec4(pow(currentFragment.rgb, vec3(1.0 / gamma)), currentFragment.a);
}
