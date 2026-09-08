/*
MIT License

Copyright (c) 2022-2026 Richard Powell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <wx/listctrl.h>
#include <wxUI/GetterSetter.hpp>
#include <wxUI/Widget.hpp>

#include <wxUI/detail/HelperMacros.hpp>

namespace wxUI {

namespace wxUI::details {

inline auto toWxListItem(auto &&text) -> wxListItem {
    auto item = wxListItem {};
    item.SetText(std::forward<decltype(text)>(text));
    return item;
}

template <details::utf8_text_choice T>
inline auto convertToListItems(std::initializer_list<T> choices) -> std::vector<wxListItem>
{
    std::vector<wxListItem> result;
    result.reserve(choices.size());
    for (auto const& choice : choices) {
        result.push_back(details::toWxListItem(choice));
    }
    return result;
}

inline auto flattenToListItems(std::initializer_list<std::initializer_list<char const*>> choices) -> std::vector<wxListItem>
{
    auto count = static_cast<size_t>(0);
    for (auto const& group : choices) {
        count += group.size();
    }
    auto result = std::vector<wxListItem> {};
    result.reserve(count);
    for (auto const& group : choices) {
        for (auto const* choice : group) {
            result.push_back(details::toWxListItem(choice));
        }
    }
    return result;
}

// Convert variadic arguments to vector of wxString
template <details::utf8_text_choice... Strings>
inline auto toVectorListItems(Strings&&... strings) -> std::vector<wxListItem>
{
    auto result = std::vector<wxListItem> {};
    result.reserve(sizeof...(strings));
    (result.push_back(details::toWxListItem(std::forward<Strings>(strings))), ...);
    return result;
}

template <utf8_text_input_range Range>
inline auto ToVectorListItems(Range&& range) -> std::vector<wxListItem>
{
    auto result = std::vector<wxListItem> {};
    if constexpr (std::ranges::sized_range<Range>) {
        result.reserve(std::ranges::size(range));
    }
    std::ranges::transform(std::forward<Range>(range), std::back_inserter(result), [](auto const& choice) {
        return details::toWxListItem(choice);
    });
    return result;
}

}

// https://docs.wxwidgets.org/latest/classwx_list_ctrl.html
struct ListCtrl {
    using underlying_t = wxListCtrl;

    ListCtrl() = default;

    template <typename String>
    requires details::utf8_text_choice<String>
    explicit ListCtrl(std::initializer_list<String> choices)
        : ListCtrl(wxID_ANY, choices)
    {
    }

    explicit ListCtrl(std::initializer_list<std::initializer_list<char const*>> choices)
        : ListCtrl(wxID_ANY, choices)
    {
    }

    explicit ListCtrl(details::Ranges::utf8_text_input_range auto&& choices)
        : ListCtrl(wxID_ANY, std::forward<decltype(choices)>(choices))
    {
    }

    template <typename... Strings>
    requires(sizeof...(Strings) > 0) && (details::utf8_text_choice<Strings> && ...)
    ListCtrl(Strings&&... choices)
        : ListCtrl(wxID_ANY, std::forward<Strings>(choices)...)
    {
    }

    explicit ListCtrl(wxWindowID identity)
        : ListCtrl(identity, std::initializer_list<char const*> {})
    {
    }

    template <typename String>
    requires details::utf8_text_choice<String>
    ListCtrl(wxWindowID identity, std::initializer_list<String> choices)
        : details_(identity)
        , items_(details::Ranges::convertToListItems(choices))
    {
    }

    ListCtrl(wxWindowID identity, std::initializer_list<std::initializer_list<char const*>> choices)
        : details_(identity)
        , items_(details::Ranges::flattenToListItems(choices))
    {
    }

    ListCtrl(wxWindowID identity, details::Ranges::utf8_text_input_range auto&& choices)
        : details_(identity)
        , items_(details::Ranges::ToVectorListItems(std::forward<decltype(choices)>(choices)))
    {
    }

    template <typename... Strings>
    requires(sizeof...(Strings) > 0) && (details::utf8_text_choice<Strings> && ...)
    ListCtrl(wxWindowID identity, Strings&&... choices)
        : details_(identity)
        , items_(details::Ranges::toVectorListItems(std::forward<Strings>(choices)...))
    {
    }

    auto withItems(std::vector<wxListItem> items) & -> ListCtrl&
    {
        items_ = std::move(items);
        return *this;
    }

    auto withItems(std::vector<wxListItem> items) && -> ListCtrl&&
    {
        items_ = std::move(items);
        return *this;
    }

    auto withColumns(std::vector<wxListItem> columns) & -> ListCtrl&
    {
        columns_ = std::move(columns);
        return *this;
    }

    auto withColumns(std::vector<wxListItem> columns) && -> ListCtrl&&
    {
        columns_ = std::move(columns);
        return *this;
    }

    auto withNormalImages(wxVector<wxListItem> images) & -> ListCtrl&
    {
        normalImages_ = std::move(images);
        return *this;
    }

    auto withNormalImages(wxVector<wxListItem> images) && -> ListCtrl&&
    {
        normalImages_ = std::move(images);
        return *this;
    }

    auto withSmallImages(wxVector<wxListItem> images) & -> ListCtrl&
    {
        smallImages_ = std::move(images);
        return *this;
    }

    auto withSmallImages(wxVector<wxListItem> images) && -> ListCtrl&&
    {
        smallImages_ = std::move(images);
        return *this;
    }

    auto setSelection(int which) & -> ListCtrl&
    {
        selection_ = { which };
        return *this;
    }

    auto setSelection(int which) && -> ListCtrl&&
    {
        selection_ = { which };
        return std::move(*this);
    }

    auto withSelection(int which) & -> ListCtrl&
    {
        selection_.push_back(which);
        return *this;
    }

    auto withSelection(int which) && -> ListCtrl&&
    {
        selection_.push_back(which);
        return std::move(*this);
    }

    auto withSelections(std::vector<int> which) & -> ListCtrl&
    {
        selection_.insert(selection_.end(), which.begin(), which.end());
        return *this;
    }

    auto withSelections(std::vector<int> which) && -> ListCtrl&&
    {
        selection_.insert(selection_.end(), which.begin(), which.end());
        return std::move(*this);
    }

    auto withEnsureVisible(int which) & -> ListCtrl&
    {
        ensureVisible_ = which;
        return *this;
    }

    auto withEnsureVisible(int which) && -> ListCtrl&&
    {
        ensureVisible_ = which;
        return std::move(*this);
    }

    template <typename Function>
    auto bind(Function func) & -> ListCtrl&
    {
        details_.bind(wxEVT_LISTBOX, func);
        return *this;
    }

    template <typename Function>
    auto bind(Function func) && -> ListCtrl&&
    {
        details_.bind(wxEVT_LISTBOX, func);
        return std::move(*this);
    }

    template <typename Function>
    auto bindDClick(Function func) & -> ListCtrl&
    {
        details_.bind(wxEVT_LISTBOX_DCLICK, func);
        return *this;
    }

    template <typename Function>
    auto bindDClick(Function func) && -> ListCtrl&&
    {
        details_.bind(wxEVT_LISTBOX_DCLICK, func);
        return std::move(*this);
    }

    struct Proxy : details::Proxy<underlying_t> {
        [[nodiscard]] auto selection() const
        {
            auto* controller = control();
            return details::GetterSetter {
                [controller] { return controller->GetSelection(); },
                [controller](int selection) { controller->SetSelection(selection); }
            };
        }

        [[nodiscard]] auto selections() const
        {
            auto* controller = control();
            return details::GetterSetter {
                [controller]() -> std::vector<int> {
                    wxArrayInt selectedItems;
                    controller->GetSelections(selectedItems);
                    return std::vector<int>(selectedItems.begin(), selectedItems.end());
                },
                [controller](std::vector<int> const& selections) {
                    controller->DeselectAll();
                    for (auto&& selection : selections) {
                        controller->SetSelection(selection);
                    }
                }
            };
        }

        auto
        operator*() const
        {
            return selection();
        }
    };

private:
    details::WidgetDetails<ListCtrl, underlying_t> details_;
    std::vector<wxListItem> columns_ {};
    std::vector<wxListItem> items_ {};
    std::vector<int> selection_;
    std::optional<int> ensureVisible_ {};
    std::optional<wxVector<wxBitmapBundle>> normalImages_ {};
    std::optional<wxVector<wxBitmapBundle>> smallImages_ {};

    template <typename Parent>
    auto createImpl()
    {
        return [&columns = columns_, 
            &items = items_,
            &normalImages = normalImages_,
            &smallImages = smallImages_,
            &selections = selection_,
            &ensureVisible = ensureVisible_](Parent* parent, wxWindowID id, wxPoint pos, wxSize size, int64_t style) {
            auto* widget = customizations::ParentCreate<underlying_t>(parent, id, pos, size, style);

            if (normalImages) {
                widget->SetNormalImages(*normalImages);
            }

            if (smallImages) {
                widget->SetSmallImages(*smallImages);
            }

            for (long index {}; auto &&column : columns) {
                widget->InsertColumn(index++, column);
            }

            for (auto &&item : items) {
                widget->InsertItem(item);
            }

            for (auto&& selection : selections) {
                widget->SetSelection(selection);
            }

            if (ensureVisible) {
                widget->EnsureVisible(*ensureVisible);
            }

            return widget;
        };
    }

public:
    WXUI_FORWARD_ALL_TO_DETAILS(ListCtrl)
};

WXUI_WIDGET_STATIC_ASSERT_BOILERPLATE(ListCtrl);
}

#include <wxUI/detail/ZapMacros.hpp>
