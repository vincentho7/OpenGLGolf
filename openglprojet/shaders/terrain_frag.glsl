#version 330 core
uniform sampler2D terrainTexture;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;



void main()
{
    // Si vos hauteurs ne sont pas déjà normalisées (dans la plage [0,1]), vous devez les normaliser
    // en divisant par la hauteur maximale. Sinon, vous pouvez simplement utiliser FragPos.y
    // Normalisation de la hauteur à l'échelle de l'amplitude
    float normalizedHeight = (FragPos.y + 8.0 -0.9) / 16.0; // Supposons que l'amplitude est 8, donc l'intervalle de hauteur est de -8 à +8

    vec3 lightDir = normalize(vec3(0.0, -1.0, 0.0)); // direction de la lumière fixe
    
    // Couleurs
    vec3 colorWater = vec3(0.0, 0.4, 1.0); // Bleu
    vec3 colorSand = vec3(1.0, 0.96, 0.62); // Sable
    vec3 colorGrassLight = vec3(0.48, 0.99, 0.0); // Vert clair
    vec3 colorGrassDark = vec3(0.3, 0.8, 0.0); // Vert foncé
    vec3 colorRock = vec3(0.91, 0.95, 0.95); // Roche
    vec3 colorSnow = vec3(1.0, 1.0, 1.0); // Blanc

    vec3 color;
    if (normalizedHeight < 0.17) {
        color = colorWater;
    } else if (normalizedHeight < 0.25) {
        color = mix(colorSand, colorGrassLight, smoothstep(0.2, 0.3, normalizedHeight));
    } else if (normalizedHeight < 0.45) {
        color = mix(colorGrassLight, colorGrassDark, smoothstep(0.3, 0.5, normalizedHeight));
    } else if (normalizedHeight < 0.65) {
        color = mix(colorGrassDark, colorRock, smoothstep(0.5, 0.7, normalizedHeight));
    } else {
        color = mix(colorRock, colorSnow, smoothstep(0.7, 1.0, normalizedHeight));
    }

    // Éclairage basé sur la pente
    float lighting = max(dot(Normal, lightDir), 0.0);
    vec3 ambient = 0.3 * color;
    vec3 diffuse = 0.7 * color * lighting;

    FragColor = vec4(ambient + diffuse, 1.0);
}