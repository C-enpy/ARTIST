# Revue indépendante des contrats GPU — candidat 8f0a2ca

Date : 2026-10-16. Auteur : correctness-reviewer / review-gpu-contracts, fresh.
PM/LLM C78/R69, grille approuvée v1. HEAD propre vérifié avant/après :
`8f0a2ca6df6266ef8783dd8e9205a9e7f333cec6`.
Treize profils et quinze skills nouveaux/enrichis/références examinés, sans
modifier ni exécuter test/build/GPU/capture/performance/probe/indexeur.

Aucun bug technique démontré dans ce périmètre documentaire. Séparations
ownership/borrow/génération/complétion, dernière utilisation multi-queue,
RAW/WAR/WAW/visibilité/attente, allocator vs command-list Reset D3D12, Metal3/4,
contexte/OpenGL3.3, authoring/cook/frame et ABI/caches jugées cohérentes. Références
Microsoft et Apple ciblées recoupées. Assets DDS BC1 5×3, protocole A/B et niveaux
de validation sans gain ni backend inventé. Pas de certification GPU exhaustive.

## Q1 — Question Vulkan, risque potentiel moyen, pas finding confirmé

`.opencode/skills/vulkan-backend/references/recipes.md:54-58,80-84`, VK-02,
préconditions VK-01:22-31. Nominal D32_SFLOAT avec DEPTH_ATTACHMENT_OPTIMAL ;
features minimales listées dynamicRendering et synchronization2, pas
separateDepthStencilLayouts. La refpage de cette feature décrit son contrôle
des layouts séparés ; les VU VkImageMemoryBarrier2 consultées conditionnent
explicitement les aspects séparés au format combiné depth+stencil, ici depth-only.
Le reviewer n'a établi ni VUID ni violation exécutée pour D32/dynamic rendering.

Arbitrage demandé : légalité normative Vulkan1.3 avec feature false/true pour
format depth-only, sans VUID inventé. Si besoin, alternative documentaire
DEPTH_STENCIL_ATTACHMENT_OPTIMAL ou activation explicite à confirmer. Une
expérience GPU future requiert logs/oracle nominal et négatif réels ; zéro warning
n'est pas une preuve suffisante. Ne pas transformer la question en bug certain.

PASS borné : cohérence documentaire examinée sous réserve Q1. INCONCLUSIVE :
API exécutée, image, découverte/enforcement et performance (hors revue).
DEFER_TO shader-reviewer pour certification mathématique/oracles image/physiques,
performance-reviewer pour toute future conclusion de gain ; aucune revendication
mesurée actuelle à certifier. Aucun autre agent lancé par le reviewer.
