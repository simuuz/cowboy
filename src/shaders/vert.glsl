out vec4 fragColor;
in vec2 fragCoord;

vec4 applyScanLines(vec4 color, vec2 coord, float number, float amount, float power, float drift)
{
  coord.y += iTime * drift;
    
  float darkenAmount = 0.5 + 0.5 * cos(coord.y * 6.28 * number);
  darkenAmount = pow(darkenAmount, power);
	
  color.rgb -= darkenAmount * amount;
    
  return color;
}

void main()
{
  vec2 sourceCoord = fragCoord.xy / iResolution.xy;
  vec4 outputColor = texture(iChannel0, fract(sourceCoord));
  fragColor = applyScanLines(outputColor, sourceCoord, 150.0, 0.25, 2.0, 0.01);
}