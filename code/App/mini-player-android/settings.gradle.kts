pluginManagement {
    repositories {
        google {
            content {
                groupByRegex {
                    "com\\.android.*"
                }
                groupByRegex {
                    "com\\.google.*"
                }
                groupByRegex {
                    "androidx.*"
                }
            }
        }
        mavenCentral()
        gradlePluginPortal()
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "MiniPlayer"
include(":app")
