# Q1 — arbitrage documentaire du layout depth Vulkan

Date : 2026-10-16. Auteur : gpu-engineer / arbitrate-vulkan-layout, lecture seule,
substitution explicite au profil Vulkan absent du répertoire d'admission.
PM/LLM brut/retenu C57/R48, investigation seulement ; politique v1 sans override.
Le worker a lu le vrai worktree d'intégration mais n'a pas recertifié son HEAD
propre : son témoignage n'est pas une preuve d'identité du candidat ni d'API.

## Observations normatives recoupées

Sources fixes Vulkan-Docs **v1.3.296**, HTTP en lecture seule via Context Mode :

- [features.adoc](https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.3.296/chapters/features.adoc),
  `features-separateDepthStencilLayouts` : la description de la feature évoque
  les aspects séparés et l'utilisation des layouts DEPTH_*/STENCIL_* en termes
  généraux, sans exception depth-only explicite dans ce passage.
- [image_layout_transition_common.adoc](https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.3.296/chapters/commonvalidity/image_layout_transition_common.adoc) :
  VUID-VkImageMemoryBarrier2-image-03320 exige un format possédant **depth et
  stencil**, puis les deux aspects si la feature est désactivée. Il ne condamne
  donc pas le cas D32_SFLOAT. Même condition de format combiné pour 03319 ;
  aspectMask-08703 porte sur l'incompatibilité avec l'aspect STENCIL, absent ici.
- [resources.adoc](https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.3.296/chapters/resources.adoc),
  Image Layouts : des aspects depth/stencil d'une même sous-ressource dans des
  layouts différents demandent la feature. Une image D32 n'a pas de stencil.
- [renderpass.adoc](https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.3.296/chapters/renderpass.adoc),
  VkRenderingInfo : aucune interdiction feature=false propre au cas D32 trouvée
  par le worker. Ne pas transposer les règles des render passes legacy au rendu
  dynamique pour fabriquer une violation.
- Corroboration secondaire, **pas une norme ni une exécution** :
  [cc_synchronization.cpp](https://raw.githubusercontent.com/KhronosGroup/Vulkan-ValidationLayers/vulkan-sdk-1.3.296.0/layers/core_checks/cc_synchronization.cpp),
  le contrôle observé est conditionné à `vkuFormatIsDepthAndStencil`.

## Verdict et décision bornée du lead

Aucune violation certaine n'est établie. L'absence de VUID trouvé ne prouve pas
la légalité globale : celle de DEPTH_ATTACHMENT_OPTIMAL sur D32 avec feature=false
reste **INCONCLUSIVE** dans cet arbitrage. Aucun bug GPU reproduit ni gain.

Choix documentaire local (PM/LLM C40/R40, sans code exécutable nouveau) : la recette
VK-02 utilise désormais **DEPTH_STENCIL_ATTACHMENT_OPTIMAL** à la transition ET
dans VkRenderingAttachmentInfo, avec image D32 et aspect DEPTH inchangés. Cette
alternative évite l'ambiguïté sans ajouter de feature minimale ; elle n'est pas
présentée comme la correction d'une violation reproduite. Les oracles rouge/vert,
usages, stages/access et les exigences de preuve API restent inchangés.

Non-régression exécutée par le lead :
`node --test --test-reporter=tap .opencode/tests/agentic/t06-vulkan/contract.test.mjs`,
exit0, **9/9**, zéro échec/skip ; `git diff --check`, exit0. Log local :
`out/agentic/mission-gpu-qa/q1-doc-nonregression.tap`. Les tests et leurs oracles
n'ont pas été modifiés. Aucune preuve API, image ou performance ne peut être
déduite de cette suite.

Expérience API future : enregistrer features/formats/layouts effectifs ; nominal
D32 + readback rouge au centre ; négatif séparé sur format **combiné** supporté,
feature=false, barrière DEPTH seule → VUID 03320 attendu. Ne jamais attendre ce
VUID pour D32. Sans couche/fixture autorisée : INCONCLUSIVE.
