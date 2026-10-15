# Métaprogrammation, statique, dynamique et data-driven dans ARTIST

## Je corrigerais une partie de ma conclusion précédente

Après avoir relu le code d’ARTIST et confronté ton intention aux modèles employés par Vulkan, Direct3D 12 et les optimisations réellement disponibles dans Clang/LLVM, je formulerais ma conclusion de manière beaucoup plus nuancée :

**non, je ne pense pas que ton idée de faire un maximum de résolution à la compilation soit mauvaise.** Au contraire, il y a une très bonne idée architecturale derrière ARTIST.

Ce que je remettrais en cause n’est pas :

> « Je veux que mon pipeline soit résolu et spécialisé avant son exécution. »

Je remettrais en cause cette version plus forte :

> « Donc toute la structure logique du moteur doit nécessairement exister sous forme de types C++ et de combinaisons de templates connues lors de la compilation du programme. »

Ce sont deux propositions très différentes.

Ton README dit explicitement que le but d’ARTIST est de résoudre les configurations de pipeline à la compilation grâce aux templates et à la métaprogrammation. Les classes `Pipeline`, `Pass`, `Shader`, `Uniform` et `Attribute` sont effectivement structurées autour de cette idée. fileciteturn7file0L2-L2

Et **je garderais cette philosophie**.

Mais je déplacerais la frontière.

Le modèle que je recommanderais est :

```text
            STATIQUE C++
                 ↓
       primitives / invariants
       backend / layouts / kernels
                 ↓
       DATA-DRIVEN AUTHORING
                 ↓
      material / render graph /
      effects / assets / passes
                 ↓
       "COMPILATION" DU DATA
                 ↓
      ExecutionPlan immuable
      PipelineState / handles
      IDs / offsets / batches
                 ↓
          HOT RUNTIME LOOP
                 ↓
      quasiment aucun lookup,
      quasiment aucune allocation,
      quasiment aucun polymorphisme
```

Autrement dit :

> **Je ne veux pas rendre ton moteur moins compilé. Je veux qu’il ait plusieurs étapes de compilation.**

C’est exactement là où je pense que la distinction est importante.

Un pipeline peut être **data-driven et néanmoins totalement aplati avant son exécution**.

Et cette distinction est très proche de ce que font les API graphiques modernes elles-mêmes. Vulkan décrit ses pipelines comme des objets construits à partir d’une description des shaders et des états fixed-function, précisément afin de permettre des optimisations inter-étages et d’éliminer de la validation coûteuse au moment du draw. Certains états restent néanmoins dynamiques. citeturn1view1 Direct3D 12 fait la même séparation : la plupart des états sont regroupés dans des Pipeline State Objects généralement préparés à l’initialisation, tandis qu’une partie des ressources, viewports, scissors ou autres valeurs continue à être fournie dynamiquement. citeturn2view0

**C’est exactement le modèle que j’appliquerais à ARTIST lui-même.**

## Ce qu’ARTIST fait réellement aujourd’hui

La chose la plus intéressante en relisant le dépôt est que **ton architecture actuelle n’est déjà pas réellement “tout statique”**.

Elle est hybride.

Et le problème principal n’est donc pas qu’il y ait « trop de templates ». Le problème est plutôt que **la frontière statique/dynamique se trouve actuellement à un endroit où tu payes parfois les deux modèles simultanément**.

Prenons `Pipeline`.

Tu as :

```cpp
template <typename API>
class IPipeline
{
public:
    virtual ~IPipeline() = default;

    virtual bool useNext();
    virtual void use(const int& pass);
    virtual void reset();

protected:
    virtual void use(
        std::shared_ptr<typename API::PipelineContext> context
    ) = 0;

    virtual void reset(
        std::shared_ptr<typename API::PipelineContext> context
    ) = 0;
};
```

Puis :

```cpp
template <typename API, auto PROFILE>
class Pipeline : public IPipeline<API>
{
protected:
    void use(...) override
    {
        if constexpr (
            HasComponent<
                typename API::PipelineContext::User<PROFILE>
            >
        )
        {
            API::PipelineContext::
                template User<PROFILE>::on(context);
        }
    }
};
```

Le cœur est donc très intéressant : `PROFILE` est un paramètre de template, le `if constexpr` sélectionne le composant pendant l’instanciation, et le composant final expose une fonction statique `on()`. fileciteturn4file0L2-L2

**Cette partie-là, je la trouve bonne.**

Elle dit en substance :

```text
API + PROFILE
      ↓
compile time
      ↓
implementation concrète
```

Le compilateur connaît effectivement :

```cpp
OpenGLPipelineUser<Pipeline::Classic>
```

et peut optimiser autour de cette implémentation.

Mais juste avant cela, tu as une fonction virtuelle :

```cpp
virtual void use(...) = 0;
```

Donc si l’appel se présente au compilateur comme :

```cpp
IPipeline<OpenGL>* p;
p->use(...);
```

il ne peut pas automatiquement supposer que `p` désigne :

```cpp
Pipeline<OpenGL, Classic>
```

Il **peut parfois le démontrer et dévirtualiser l’appel**, surtout avec une visibilité whole-program/LTO. Clang expose explicitement `-fwhole-program-vtables` pour effectuer des optimisations comme la dévirtualisation lorsqu’une implémentation unique peut être établie, et possède également une dévirtualisation spéculative destinée notamment à ouvrir de nouvelles possibilités d’inlining. citeturn3view1turn3view2

Mais ce n’est plus la garantie structurelle que tu recherchais.

Même phénomène un étage plus bas.

`PipelineContext` contient :

```cpp
std::vector<std::shared_ptr<pipeline::IPass<API>>> m_passes;
```

et renvoie des `shared_ptr<IPass<API>>`. fileciteturn14file0L2-L2

`IPass` possède à son tour des fonctions virtuelles :

```cpp
virtual void load();
virtual void use();
virtual void free();

virtual void load(context) = 0;
virtual void readUniforms(context) = 0;
virtual void readAttributes(context) = 0;
virtual void free(context) = 0;
virtual void use(context) = 0;
```

puis `Pass<API, PROFILE>` retombe à nouveau sur tes composants statiques sélectionnés via `if constexpr`. fileciteturn5file0L2-L2

Et encore un étage en dessous :

```text
Pipeline
    ↓ virtual/shared_ptr
Pass
    ↓ virtual/shared_ptr
Shader
    ↓ virtual/shared_ptr
component statique
```

`IShader` possède lui aussi une interface virtuelle et stocke son contexte dans un `std::shared_ptr`; `Shader<API, PROFILE>` appelle ensuite les `Loader<PROFILE>`, `Freer<PROFILE>` et `Reader<PROFILE>` statiques. fileciteturn10file0L2-L2

Donc je décrirais l’ARTIST actuel comme ceci :

```text
                       compile-time
                          ↓
      API + PROFILE → specialization
                          ↓
                    static component
                          ↑
                 virtual function
                          ↑
                    shared_ptr
                          ↑
              runtime object graph
```

C’est là que se trouve mon objection.

**Tu utilises le statique pour spécialiser les feuilles, mais tu reviens ensuite au polymorphisme dynamique pour traverser presque tous les étages de l’arbre.**

Tu ne récupères donc pas forcément tout le bénéfice du graphe de types que tu as construit.

### Le contraste est encore plus visible avec les uniforms et attributes

Ton `Uniform::set<T>()` est, à mon avis, une utilisation particulièrement intéressante de la métaprogrammation :

```cpp
template <typename T>
void set(const T& value)
{
    if constexpr (
        HasComponent<
            typename API::UniformContext::template Setter<T>
        >
    )
    {
        m_context->template setValue<T>(value);
        API::UniformContext::template Setter<T>::on(m_context);
    }
}
```

Ici, le type du setter est réellement résolu au compile-time. fileciteturn11file0L2-L2

**Je garderais ce principe.**

Mais l’uniform est récupéré par son nom dans :

```cpp
std::unordered_map<
    std::string,
    std::shared_ptr<Uniform<API>>
>
```

au niveau du `PassContext`. Les shaders sont eux-mêmes dans un `vector<shared_ptr<IShader<API>>>`, et les attributes suivent le même modèle de map par chaîne. fileciteturn8file0L2-L2

Et `AttributeContext` utilise :

```cpp
std::any m_value;
```

avec des :

```cpp
std::any_cast<std::shared_ptr<T>>
```

à l'exécution. fileciteturn9file0L2-L2

On obtient alors quelque chose d’un peu paradoxal :

```text
name string
   ↓
unordered_map
   ↓
shared_ptr
   ↓
runtime type-erased storage
   ↓
template<T>
   ↓
static specialization
```

Je pense que **c’est précisément cette inversion-là que tu dois corriger**, plutôt que d’abandonner la métaprogrammation.

## Le point fondamental : statique, dynamique et data-driven ne s’opposent pas

Je crois que c’est ici que notre discussion précédente pouvait être trompeuse.

Il y a en réalité au moins **quatre moments différents auxquels un moteur peut “compiler” quelque chose**.

| Moment | Exemple | Qui connaît la décision ? |
|---|---|---|
| Compilation C++ | `Renderer<Vulkan>`, SIMD, layouts, concepts | compilateur C++ |
| Build des assets | shader permutations, material graph, formats | asset compiler |
| Initialisation / chargement | PSO, render graph, bindings, handles | moteur |
| Frame | transforms, visibilité, animations, lights | runtime |

Le piège est de considérer uniquement le premier comme « statique ».

Ce n’est pas nécessaire.

Vulkan lui-même illustre parfaitement cela. Un `VkPipeline` n’existe évidemment pas sous forme de template C++ dans ton exécutable. Pourtant l’API encourage sa construction avant les draws : shaders et fixed-function state sont regroupés dans un objet de pipeline afin que l’implémentation puisse effectuer des optimisations et éviter de refaire certaines validations au draw time. citeturn1view1

Et comme cette création peut être coûteuse, Vulkan possède explicitement des caches de pipelines permettant de réutiliser les résultats entre plusieurs créations et même entre plusieurs exécutions de l’application. La documentation Khronos mentionne notamment la compilation des shaders comme une partie potentiellement coûteuse de la création. citeturn1view0

Direct3D 12 formule la même philosophie très explicitement : les PSO ont été conçus pour que les dépendances entre états puissent être prétraitées généralement à l’initialisation, rendant leur changement au render time aussi efficace que possible. citeturn2view0

Donc :

> **“Data-driven” ne veut pas dire “interpréter des JSON et faire des hash maps pendant chaque draw”.**

Ça, ce serait une mauvaise architecture.

Un bon moteur data-driven fait plutôt :

```text
JSON/YAML/binary/material graph
             ↓
         validation
             ↓
        resolution
             ↓
        compilation
             ↓
      RuntimeMaterial
             ↓
 PipelineID = 127
 TextureID  = 827
 BufferSlot = 4
 Offset     = 256
 Flags      = 0x24
```

Et pendant la frame :

```cpp
auto& material = materials[materialId];

cmd.bindPipeline(material.pipeline);
cmd.bindDescriptors(material.descriptors);
cmd.draw(mesh);
```

À ce stade-là il n’y a plus besoin de :

```cpp
unordered_map<string, ...>
std::any
dynamic_cast
material graph traversal
shader graph traversal
file parsing
```

Le **data-driven est en amont**.

Le résultat du data-driven est **compilé en une représentation runtime extrêmement pauvre, compacte et statique dans sa structure**.

Voilà la modification fondamentale que je ferais à ton idée.

### C++ compile-time versus engine compile-time

Je donnerais deux noms différents aux choses :

```text
C++ static specialization
```

et

```text
runtime-plan compilation
```

Ce sont deux armes complémentaires.

Par exemple :

```cpp
template<GraphicsBackend Backend>
struct CommandEncoder;
```

est une excellente spécialisation C++.

En revanche :

```cpp
using SquallSkinHairShadowPipeline =
    Pipeline<
        Vulkan,
        Pass<
            Character,
            Skinning<4>,
            Hair<Simple>,
            Shadow<PCF>,
            Alpha<Opaque>,
            ...
        >
    >;
```

commence à encoder dans ton système de types **du contenu de jeu**, et c’est là que le modèle peut exploser.

Le standard C++ demande bien l’instanciation de spécialisations de templates lorsque celles-ci sont nécessaires ; multiplier les combinaisons de types multiplie donc les spécialisations que le compilateur doit traiter. citeturn5view0turn5view1 Clang fournit d’ailleurs `-ftime-trace` et des rapports de consommation mémoire/temps précisément pour mesurer où part le coût de compilation, tandis que certaines formes de LTO plus agressives augmentent elles-mêmes les besoins en temps de link et en mémoire. citeturn3view0turn3view1

Imagine seulement douze caractéristiques binaires indépendantes :

```text
skinned
morph
alpha
double-sided
normal-map
emissive
hair
cloth
shadow
reflection
fog
decal
```

L’espace théorique est :

\[
2^{12}=4096
\]

combinaisons.

Tu n'auras probablement jamais besoin des 4096, mais **c’est exactement la raison pour laquelle je ne veux pas que le produit cartésien du contenu de FFVIII devienne le produit cartésien de tes instanciations C++**.

Je veux que ton C++ sache implémenter :

```text
skinning
hair
cloth
shadowing
alpha
...
```

Je ne veux pas nécessairement qu’il construise un **type C++ distinct pour chaque mélange de ces caractéristiques**.

## Ce que je garderais statique, dynamique et data-driven

Je tracerais la frontière de la façon suivante.

| Système | Mon choix | Pourquoi |
|---|---|---|
| Backend Vulkan/D3D12/Metal | **statique** | ensemble petit, fermé, impact majeur sur le code |
| Types GPU fondamentaux | **statique** | contraintes connues et vérifiables |
| Vertex layouts | **statique ou généré** | permet des interfaces fortement typées |
| Uniform/storage structs | **statique/généré** | offsets et types connus |
| SIMD/math/kernels | **statique** | spécialisation réellement utile |
| Allocateurs spécialisés | **statique** | structure et invariants connus |
| Skinning kernel | **statique** | quelques implementations fermées |
| Cloth/hair solver kernels | **statique** | algorithms spécialisés |
| Validation de composants | **concepts/statique** | parfait usage de `requires` |
| Liste de matériaux | **data-driven** | c'est du contenu |
| Meshes/textures | **data-driven** | assets |
| Paramètres PBR | **data-driven** | contenu éditable |
| Graphes d'animation | **data-driven** | besoin d'itération |
| Particle effects | **data-driven** | contenu artistique |
| Hair configurations | **data-driven** | paramètres par personnage |
| Cloth configurations | **data-driven** | paramètres par costume |
| Render graph | **data-driven puis compilé** | adaptable mais pas interprété dans le hot path |
| Pipeline states | **data-driven puis compilé/cache** | correspond au modèle Vulkan/D3D12 |
| Transformations | **dynamique** | changent chaque frame |
| Bone poses | **dynamique** | changent chaque frame |
| Visibility | **dynamique** | dépend de la caméra |
| Lights | **dynamique** | scène runtime |
| Particle state | **dynamique** | simulation |
| Cloth/hair particles | **dynamique** | simulation |
| Streaming state | **dynamique** | dépend de la mémoire/position |

La règle que j’utiliserais est extrêmement simple :

> **Un choix doit devenir un paramètre de template lorsque la spécialisation modifie réellement le code machine que tu souhaites générer, que l’ensemble des possibilités est petit et fermé, et que la décision est légitimement connue lors du build C++.**

En revanche :

> **Une différence de contenu ne devrait presque jamais créer un nouveau type C++.**

Squall et Zell ne devraient pas être deux types de renderer.

Un matériau métallique et un matériau en cuir ne devraient probablement pas être deux types C++.

Deux coiffures ne devraient certainement pas être deux instanciations d'un système.

Mais :

```cpp
HairSolver<Verlet>
HairSolver<XPBD>
```

peut parfaitement avoir du sens.

Parce que là tu spécialises **l'algorithme**, pas **l'asset**.

### Le `PROFILE` d’ARTIST est donc une bonne idée, avec une condition

Aujourd’hui tu as par exemple :

```cpp
enum class Pipeline
{
    Classic
};
```

puis des spécialisations de composants autour de ce profil. fileciteturn13file0L2-L16

Cela reste parfaitement raisonnable tant que `PROFILE` représente quelque chose du genre :

```text
Classic
MeshShader
RayTracing
Mobile
Deferred
Forward
```

ou :

```text
OpenGLClassic
VulkanBindless
VulkanMesh
```

c’est-à-dire **un nombre faible de stratégies d’implémentation réellement différentes**.

Je deviendrais beaucoup plus méfiant si `PROFILE` finit par devenir :

```text
CharacterHairSkinAlphaShadowPBR
CharacterNoHairSkinAlphaShadowPBR
CharacterHairSkinOpaqueShadowPBR
CharacterHairSkinOpaqueNoShadowPBR
...
```

Parce qu’à ce moment-là tu n’es plus en train de spécialiser ton renderer.

Tu es en train d’encoder ta database de matériaux dans le système de types.

## Le coût réel de chaque décision

Il faut surtout éviter la caricature « virtual = lent, template = rapide ».

La réalité est plus intéressante.

### Templates et polymorphisme statique

Le gros avantage n’est pas seulement d’économiser l’instruction `call`.

Le gros avantage est que le compilateur peut voir la cible exacte et donc effectuer :

```text
inline
constant propagation
dead-code elimination
branch elimination
loop transformations
alias analysis
vectorization opportunities
```

Clang décrit explicitement la dévirtualisation comme utile notamment parce qu’une cible devenue directe ouvre davantage de possibilités d’inlining et d’optimisation. citeturn3view1

Donc ton intuition :

> « si le pipeline entier est visible, il peut être aplati »

est **correcte**.

Là où je mets le frein est :

> « donc toute abstraction dynamique doit disparaître ».

Non.

Une virtual call faite **une fois par frame** pour sélectionner le backend n’a pratiquement aucune importance architecturale.

Une virtual call faite :

```text
3 fois
× 100 000 particules
× 60 FPS
```

est une autre histoire.

Le principe devrait être :

> **Statique à l'intérieur du hot loop. Dynamique autour du hot loop.**

Par exemple, ceci est excellent :

```cpp
solver->simulate(clothWorld);
```

si `simulate()` choisit une fois l'implémentation puis traite 200 000 contraintes dans une boucle spécialisée.

Ceci est beaucoup moins intéressant :

```cpp
for (Constraint* c : constraints)
    c->virtualSolve();
```

car la frontière polymorphe se trouve maintenant **dans la boucle de données**.

### Les fonctions virtuelles ne sont pas toujours virtuelles dans le code machine final

C'est aussi important pour ta réflexion.

Si le compilateur est capable de prouver la cible, il peut dévirtualiser.

Clang possède notamment des optimisations whole-program utilisant LTO ainsi qu'une dévirtualisation spéculative. citeturn3view1

Donc :

```cpp
Base& b = concrete;
b.foo();
```

n'implique pas automatiquement qu'il subsistera un indirect call dans le binaire final.

Mais je ne bâtirais pas non plus toute l’architecture sur l’espoir que le compilateur dévirtualise systématiquement.

Ta philosophie static polymorphism reste donc pertinente pour les endroits où tu **veux explicitement rendre cette connaissance disponible**.

### Le vrai problème des `shared_ptr` dans ARTIST n’est pas qu’ils « cassent le compile-time »

Je voudrais également corriger cette idée.

Un `shared_ptr<T>` n’empêche aucunement `T` d’être un type statiquement connu.

Le problème est différent.

`shared_ptr` exprime :

> **shared ownership**.

Le standard le définit explicitement comme un pointeur à propriété partagée : plusieurs `shared_ptr` appartiennent au même groupe de propriété, et la destruction du dernier propriétaire libère la ressource. Les copies partagent cette propriété et l'état du compte d'owners doit donc être maintenu. citeturn6view0

Dans ARTIST tu les utilises pratiquement comme **pointeur universel par défaut** :

```text
Pipeline → shared_ptr<PipelineContext>
PipelineContext → shared_ptr<IPass>
Pass → shared_ptr<PassContext>
PassContext → shared_ptr<IShader>
PassContext → shared_ptr<Uniform>
PassContext → shared_ptr<IAttribute>
Shader → shared_ptr<ShaderContext>
Uniform → shared_ptr<UniformContext>
...
```

C’est visible dans les implémentations de `Pipeline`, `Pass`, `Shader` et des contexts. fileciteturn4file0L2-L2 fileciteturn5file0L2-L2 fileciteturn10file0L2-L2 fileciteturn8file0L2-L2

La question que je poserais n’est pas :

> « Est-ce lent ? »

mais :

> « Est-ce réellement une relation de shared ownership ? »

Très souvent, dans un moteur, la réponse sera non.

Par exemple :

```text
Renderer owns PipelineCache
PipelineCache owns Pipelines
Material refers to Pipeline
DrawPacket refers to Material
```

Il n’y a pas besoin que `DrawPacket` possède le pipeline.

Il a juste besoin de quelque chose comme :

```cpp
PipelineHandle
```

ou :

```cpp
Pipeline*
```

avec une lifetime garantie ailleurs.

Un système moderne peut donc avoir :

```cpp
struct PipelineHandle
{
    uint32_t index;
    uint32_t generation;
};
```

et :

```cpp
Pipeline& PipelinePool::get(PipelineHandle);
```

Aucune refcount dans le hot path.

Ownership explicite.

Très bonne densité mémoire.

Possibilité de déplacer les objets en pools.

### `std::any` est une frontière dynamique beaucoup plus importante

Pour `AttributeContext`, tu as :

```cpp
std::any m_value;
```

et tu redemandes ensuite le type :

```cpp
any_cast<shared_ptr<T>>
```

avec gestion d'erreur runtime. fileciteturn9file0L2-L2

Ça, je l'éviterais complètement dans le renderer runtime.

Je préférerais quelque chose comme :

```cpp
enum class VertexSemantic : uint8_t
{
    Position,
    Normal,
    Tangent,
    UV0,
    UV1,
    Joints,
    Weights
};

struct VertexAttribute
{
    VertexSemantic semantic;
    Format format;
    uint16_t offset;
    uint8_t stream;
};
```

puis une description résolue :

```cpp
struct CompiledVertexLayout
{
    PipelineVertexLayoutHandle gpuLayout;
    std::array<VertexBinding, N> bindings;
};
```

Les formats et mappings pourraient être **validés et générés statiquement**, alors que les offsets/handles restent des données compactes.

C’est un meilleur hybride.

### Ma matrice de coût serait donc celle-ci

| Technique | Runtime | Build C++ | Taille code | Flexibilité | Hot reload |
|---|---:|---:|---:|---:|---:|
| Template specialization | excellent | cher | risque élevé | faible | mauvais |
| `if constexpr` | excellent | moyen/cher | variable | faible | mauvais |
| CRTP | excellent | moyen | variable | faible | mauvais |
| virtual dispatch | petit coût indirect | faible | faible | excellente | bonne |
| function pointer | petit coût indirect | faible | faible | excellente | bonne |
| `std::variant` + switch | très bon | moyen | moyen | ensemble fermé | moyenne |
| Handle/ID | excellent | faible | faible | excellente | excellente |
| data-driven interprété | moyen/mauvais | excellent | faible | excellente | excellente |
| data-driven compilé | **excellent** | excellent | faible | **excellente** | **excellente** |
| string lookup hot path | à éviter | faible | faible | excellente | excellente |
| `std::any` hot path | à éviter | faible | faible | excellente | excellente |
| `shared_ptr` partout | variable | faible | faible | simple | bonne |

Le point le plus important du tableau est vraiment :

> **data-driven compilé**.

C’est celui qui te donne presque tout.

## L’architecture que je construirais à partir d’ARTIST

Je ne réécrirais pas ARTIST en framework entièrement dynamique.

Je ferais presque le contraire.

Je conserverais son ADN statique, mais je créerais une frontière claire entre :

```text
implementation
```

et :

```text
configuration
```

### Le backend resterait fortement statique

Par exemple :

```cpp
template<typename Backend>
class Renderer
{
public:
    void render(const CompiledScene& scene)
    {
        ...
    }
};
```

avec :

```cpp
Renderer<VulkanBackend>
```

ou éventuellement un choix au build :

```cpp
using Renderer =
#if ARTIST_VULKAN
    RendererImpl<VulkanBackend>;
#elif ARTIST_D3D12
    RendererImpl<D3D12Backend>;
#endif
```

Dans ce scénario, **aucun virtual call n’est nécessaire dans le renderer interne**.

C’est exactement un endroit où ta philosophie est excellente : le backend ne change pas toutes les trois instructions.

### Tes composants actuels pourraient également rester statiques

Par exemple :

```cpp
template<typename Backend>
struct DrawIndexedCommand
{
    static ARTIST_FORCE_INLINE
    void execute(
        typename Backend::CommandBuffer& cmd,
        const DrawIndexedArgs& args
    )
    {
        Backend::drawIndexed(cmd, args);
    }
};
```

et :

```cpp
template<typename Backend>
struct BindPipelineCommand
{
    static ARTIST_FORCE_INLINE
    void execute(
        typename Backend::CommandBuffer& cmd,
        PipelineHandle pipeline
    )
    {
        Backend::bindPipeline(cmd, pipeline);
    }
};
```

Là, tu exploites exactement ce que tu voulais au départ.

Le compiler voit :

```cpp
VulkanBackend::bindPipeline(...)
```

et non :

```cpp
virtual IGpu::bindPipeline(...)
```

### En revanche, le pipeline artistique serait une donnée

Par exemple :

```cpp
struct MaterialDesc
{
    ShaderAsset vertexShader;
    ShaderAsset fragmentShader;

    BlendMode blend;
    CullMode cull;
    DepthMode depth;

    bool skinning;
    bool alphaTest;
    bool receiveShadow;
};
```

Ce `MaterialDesc` est flexible.

Il peut venir d’un fichier.

Il peut être hot-reloadé.

Mais on ne le manipule **jamais comme ça pendant les draws**.

On fait :

```cpp
CompiledMaterial compile(
    const MaterialDesc& desc,
    PipelineCompiler& compiler
);
```

qui produit :

```cpp
struct CompiledMaterial
{
    PipelineHandle pipeline;
    DescriptorLayoutHandle descriptorLayout;

    uint32_t materialBufferOffset;
    uint32_t flags;
};
```

Puis le renderer ne voit plus que ça.

### Même chose avec ton render graph

Authoring :

```text
DepthPrePass
    writes Depth

GBuffer
    reads Meshes
    writes Albedo, Normal, Material

Lighting
    reads Albedo, Normal, Material, Depth
    writes HDR

Transparent
    reads Depth
    writes HDR

PostProcess
    reads HDR
    writes Backbuffer
```

Puis :

```cpp
CompiledRenderGraph graph =
    renderGraphCompiler.compile(description);
```

La compilation calcule :

```text
dependencies
resource lifetimes
barriers
transient aliases
pass ordering
queue transitions
pipeline handles
descriptor bindings
```

Et la frame exécute simplement :

```cpp
graph.execute(frame);
```

Tu obtiens ainsi quelque chose de conceptuellement très proche du pipeline Vulkan : une description flexible est transformée en état prétraité afin de réduire le travail pendant l'exécution. citeturn1view1turn1view0

### Et je déplacerais les strings complètement hors du hot path

Aujourd’hui :

```cpp
pass->withUniform("modelMatrix", matrix);
```

peut finir dans une recherche de map par nom avant d’arriver sur le setter typé. fileciteturn5file0L2-L2 fileciteturn8file0L2-L2

Je voudrais que l’authoring puisse continuer à dire :

```cpp
material.set("roughness", 0.7f);
```

mais uniquement au chargement/édition.

La compilation produit :

```cpp
UniformHandle roughness;
```

puis :

```cpp
set(roughness, 0.7f);
```

ou mieux encore :

```cpp
struct CharacterMaterialParameters
{
    Mat4 model;
    Vec4 baseColor;
    float roughness;
    float metallic;
};
```

dont le layout correspond directement à ton UBO/SSBO.

Avec de la génération de code depuis ta réflexion shader, tu peux même obtenir une interface à la fois :

```text
data-driven côté shader
```

et :

```text
fortement typée côté C++
```

Ce serait, à mon sens, beaucoup plus proche de l’ambition originale d’ARTIST que `unordered_map<string, shared_ptr<Uniform>>`.

## Là où je placerais exactement la frontière pour ton FFVIII Remake

Pour ton moteur en particulier, je serais donc **beaucoup plus favorable à la métaprogrammation que ma conclusion précédente pouvait le laisser entendre**.

Je résumerais la philosophie en une phrase :

> **Compile les algorithmes en C++. Compile le contenu avec ton moteur. N’interprète ni l’un ni l’autre dans le hot path.**

Pour Squall, par exemple :

```text
                           C++ BUILD
                              │
                ┌─────────────┼─────────────┐
                │             │             │
           VulkanBackend  XpbdSolver   SkinningSIMD
                │             │             │
                └─────────────┼─────────────┘
                              │
                        ASSET BUILD
                              │
              Squall.material / hair / cloth
                              │
                              ▼
                    PipelineCompiler
                    MaterialCompiler
                    HairCompiler
                    ClothCompiler
                              │
                              ▼
                     COMPILED RUNTIME DATA
                              │
              ┌───────────────┼───────────────┐
              │               │               │
         PipelineID=8     HairConfig=3    ClothConfig=7
              │               │               │
              └───────────────┼───────────────┘
                              ▼
                           FRAME
                              │
                   transform / bones /
                   particles / cloth
                              │
                              ▼
                         GPU commands
```

Le personnage est data-driven.

Le solver XPBD est statique.

Les paramètres XPBD sont data-driven.

La position des particules est dynamique.

Le format de ces particules est statique.

Le nombre de mèches est data-driven.

Les positions des mèches sont dynamiques.

Le kernel qui les simule est statique.

Le shader est compilé offline.

La sélection du shader est data-driven.

Le PSO résultant est précalculé/caché.

Le handle de PSO est une simple donnée runtime.

**C’est ce mélange-là que je considère idéal.**

Et c'est finalement assez proche de ce que les API modernes imposent elles-mêmes : certaines décisions structurantes sont préparées dans des pipelines/PSO, alors que les ressources et certaines valeurs restent dynamiques. Vulkan permet explicitement d'avoir un pipeline largement préconfiguré tout en marquant certains états comme dynamiques ; D3D12 place de même la majeure partie de l'état dans le PSO mais laisse les bindings de ressources, viewports, scissors et quelques autres états en dehors. citeturn1view1turn2view0

### Donc, très concrètement, ce que je supprimerais d’ARTIST

Je ne supprimerais **pas** :

```cpp
template<typename API, auto PROFILE>
```

Je ne supprimerais pas :

```cpp
if constexpr (...)
```

Je ne supprimerais pas :

```cpp
requires(...)
```

Je ne supprimerais pas :

```cpp
API::FooContext::template Component<PROFILE>::on(...)
```

Au contraire, ce sont probablement les parties que je chercherais à conserver et à rendre plus cohérentes. Le système de concepts actuel est explicitement construit pour vérifier l’existence des composants et de leurs méthodes statiques. fileciteturn12file0L2-L2

Je remettrais en question :

```cpp
IPipeline
IPass
IShader
```

comme hiérarchie virtuelle **au milieu même** du chemin que tu veux rendre statiquement aplati.

Je remettrais fortement en question :

```cpp
shared_ptr
```

comme type d’ownership par défaut partout.

Je sortirais du hot path :

```cpp
unordered_map<string, ...>
```

et :

```cpp
std::any
```

qui apparaissent aujourd’hui dans les contexts des passes et attributs. fileciteturn8file0L2-L2 fileciteturn9file0L2-L2

Et je remplacerais progressivement ces éléments par :

```text
Handle
ID
index
offset
span
arena
pool
typed layout
compiled descriptor
execution plan
```

### Je ferais même un test avant toute grosse réécriture

Ton hypothèse de performance est testable.

Je construirais trois mini-renderers identiques :

```text
A. ARTIST actuel
   virtual + shared_ptr

B. ARTIST statique
   composition template complète

C. ARTIST hybride
   data-driven → compiled plan → handles
```

Et je mesurerais :

```text
CPU frame time
cycles / draw preparation
instructions retired
branch misses
I-cache misses
D-cache misses
binary size
full compile time
incremental compile time
link time
peak compiler RAM
```

Pour la partie compilation, Clang peut produire directement des traces de temps via `-ftime-trace` et des statistiques de temps/mémoire de chaque étape. citeturn3view0 Pour la partie optimisation finale, PGO peut également informer l’inliner sur les fonctions et branches réellement chaudes, ce qui est particulièrement pertinent avant de conclure qu'une abstraction dynamique donnée est nécessairement un problème. citeturn2view1

Je soupçonne fortement que le résultat intéressant ne sera pas :

```text
100 % templates gagne
```

ou :

```text
100 % runtime gagne
```

mais :

```text
coarse-grained dynamic choice
            +
statically-specialized inner kernels
            +
data-driven compiled representation
```

Et c’est finalement le changement le plus important que j’apporterais à ma conclusion précédente :

> **ARTIST ne devrait pas renoncer à son idée de “compile-time resolution”. Il devrait l’étendre en une philosophie de “resolve once, execute cheaply”.**

Le C++ compile-time résout ce qui appartient réellement au programme.

L’asset build résout ce qui appartient au contenu.

Le pipeline compiler résout ce qui dépend du GPU.

Le graph compiler résout ce qui dépend de la configuration de rendu.

Et la frame ne résout presque rien.

C’est, selon moi, la version mature de l’idée que tu avais déjà commencée à implémenter dans ARTIST.