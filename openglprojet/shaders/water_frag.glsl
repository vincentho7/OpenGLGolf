#version 330 core

out vec4 FragColor;

in vec3 Normal;  // Normale du fragment.
in vec3 FragPos;  // Position du fragment.

uniform vec3 viewPos;  // Position de la caméra.
uniform vec3 lightColor;  // Couleur de la lumière.
uniform vec3 lightPos;  // Position de la lumière.

void main()
{
    // Normaliser les vecteurs
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Calculer l'angle d'incidence
    float incidenceAngle = dot(viewDir, normal);

    // Calculer le coefficient de Fresnel
    float fresnel = pow(1.0 - incidenceAngle, 3.0);

    // Calculer la réflexion spéculaire
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    // Mélanger les couleurs de la lumière réfléchie et de l'eau transparente
    vec3 baseColor = vec3(0.0, 0.4, 0.7);
    vec3 finalColor = mix(baseColor, baseColor + specular, fresnel);

    // Ajouter la transparence en fonction de l'angle d'incidence
    float transparency = 1.0 - 1 * incidenceAngle;

    FragColor = vec4(finalColor, transparency);
}